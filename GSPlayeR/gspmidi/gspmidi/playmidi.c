/*

TiMidity -- Experimental MIDI to WAVE converter
Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

playmidi.c -- random stuff in need of rearrangement

*/

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32_WCE
#include <string.h>
#endif

#include "config.h"
#include "common.h"
#include "instrum.h"
#include "playmidi.h"
#include "readmidi.h"
#include "output.h"
#include "mix.h"
#include "tables.h"

Channel channel[16];
Voice voice[MAX_VOICES];

int
voices=DEFAULT_VOICES;

int32
control_ratio = DEFAULT_RATE / 1000,
amplification = DEFAULT_AMPLIFICATION;

FLOAT_T master_volume;

int32 drumchannels=DEFAULT_DRUMCHANNELS;
int adjust_panning_immediately=1;

static int32 lost_notes = 0, cut_notes = 0;
static int32 common_buffer[AUDIO_BUFFER_SIZE*2], /* stereo samples */
*buffer_pointer;
static int32 buffered_count = 0;

static MidiEvent *event_list, *current_event;
static int32 sample_count = 0, current_sample = 0;
static int32 compute_sample = 0;

static void adjust_amplification(void)
{
    master_volume = (double)(amplification) / 100.0L;
}

static void reset_voices(void)
{
    int i;
    for (i=0; i<MAX_VOICES; i++)
		voice[i].status=VOICE_FREE;
}

/* Process the Reset All Controllers event */
static void reset_controllers(int c)
{
    channel[c].volume=90; /* Some standard says, although the SCC docs say 0. */
    channel[c].expression=127; /* SCC-1 does this. */
    channel[c].sustain=0;
    channel[c].pitchbend=0x2000;
    channel[c].pitchfactor=0; /* to be computed */
}

static void redraw_controllers(int c)
{
    //ctl->volume(c, channel[c].volume);
    //ctl->expression(c, channel[c].expression);
    //ctl->sustain(c, channel[c].sustain);
    //ctl->pitch_bend(c, channel[c].pitchbend);
}

static void reset_midi(void)
{
    int i;
    for (i=0; i<16; i++)
    {
        reset_controllers(i);
        /* The rest of these are unaffected by the Reset All Controllers event */
        channel[i].program=default_program;
        channel[i].panning=NO_PANNING;
        channel[i].pitchsens=2;
        channel[i].bank=0; /* tone bank or drum set */
    }
    reset_voices();
}

static void select_sample(int v, Instrument *ip)
{
    int32 f, cdiff, diff;
    int s,i;
    Sample *sp, *closest;
    
    s=ip->samples;
    sp=ip->sample;
    
    if (s==1)
    {
        voice[v].sample=sp;
        return;
    }
    
    f=voice[v].orig_frequency;
    for (i=0; i<s; i++)
    {
        if (sp->low_freq <= f && sp->high_freq >= f)
        {
            voice[v].sample=sp;
            return;
        }
        sp++;
    }
    
    /*
    No suitable sample found! We'll select the sample whose root
    frequency is closest to the one we want. (Actually we should
    probably convert the low, high, and root frequencies to MIDI note
    values and compare those.) */
    
    cdiff=0x7FFFFFFF;
    closest=sp=ip->sample;
    for(i=0; i<s; i++)
    {
        diff=sp->root_freq - f;
        if (diff<0) diff=-diff;
        if (diff<cdiff)
        {
            cdiff=diff;
            closest=sp;
        }
        sp++;
    }
    voice[v].sample=closest;
    return;
}

static void recompute_freq(int v)
{
    int
    sign=(voice[v].sample_increment < 0), /* for bidirectional loops */
    pb=channel[voice[v].channel].pitchbend;
    double a;
    
    if (!voice[v].sample->sample_rate)
    return;
    
    if (voice[v].vibrato_control_ratio)
    {
        /* This instrument has vibrato. Invalidate any precomputed
        sample_increments. */
        
        int i=VIBRATO_SAMPLE_INCREMENTS;
        while (i--)
        voice[v].vibrato_sample_increment[i]=0;
    }
    
    if (pb==0x2000 || pb<0 || pb>0x3FFF)
    voice[v].frequency=voice[v].orig_frequency;
    else
    {
        pb-=0x2000;
        if (!(channel[voice[v].channel].pitchfactor))
        {
            /* Damn. Somebody bent the pitch. */
            int32 i=pb*channel[voice[v].channel].pitchsens;
            if (pb<0)
            i=-i;
            channel[voice[v].channel].pitchfactor=
            bend_fine[(i>>5) & 0xFF] * bend_coarse[i>>13];
        }
        if (pb>0)
        voice[v].frequency=
        (int32)(channel[voice[v].channel].pitchfactor *
        (double)(voice[v].orig_frequency));
        else
        voice[v].frequency=
        (int32)((double)(voice[v].orig_frequency) /
        channel[voice[v].channel].pitchfactor);
    }
    
    a = FSCALE(((double)(voice[v].sample->sample_rate) *
    (double)(voice[v].frequency)) /
    ((double)(voice[v].sample->root_freq) *
    (double)(play_mode->rate)),
    FRACTION_BITS);
    
    if (sign)
    a = -a; /* need to preserve the loop direction */
    
    voice[v].sample_increment = (int32)(a);
}

static void recompute_amp(int v)
{
    int32 tempamp;
    
    /* TODO: use fscale */
    
    tempamp= (voice[v].velocity *
    channel[voice[v].channel].volume *
    channel[voice[v].channel].expression); /* 21 bits */
    
    if (!(play_mode->encoding & PE_MONO))
    {
        if (voice[v].panning > 60 && voice[v].panning < 68)
        {
            voice[v].panned=PANNED_CENTER;
            
            voice[v].left_amp=
            FSCALENEG((double)(tempamp) * voice[v].sample->volume * master_volume,
            21);
        }
        else if (voice[v].panning<5)
        {
            voice[v].panned = PANNED_LEFT;
            
            voice[v].left_amp=
            FSCALENEG((double)(tempamp) * voice[v].sample->volume * master_volume,
            20);
        }
        else if (voice[v].panning>123)
        {
            voice[v].panned = PANNED_RIGHT;
            
            voice[v].left_amp= /* left_amp will be used */
            FSCALENEG((double)(tempamp) * voice[v].sample->volume * master_volume,
            20);
        }
        else
        {
            voice[v].panned = PANNED_MYSTERY;
            
            voice[v].left_amp=
            FSCALENEG((double)(tempamp) * voice[v].sample->volume * master_volume,
            27);
            voice[v].right_amp=voice[v].left_amp * (voice[v].panning);
            voice[v].left_amp *= (double)(127-voice[v].panning);
        }
    }
    else
    {
        voice[v].panned=PANNED_CENTER;
        
        voice[v].left_amp=
        FSCALENEG((double)(tempamp) * voice[v].sample->volume * master_volume,
        21);
    }
}

static void start_note(MidiEvent *e, int i)
{
    Instrument *ip;
    int j;
    
    if (ISDRUMCHANNEL(e->channel))
    {
        if (!(ip=drumset[channel[e->channel].bank]->tone[e->a].instrument))
        {
            if (!(ip=drumset[0]->tone[e->a].instrument))
            return; /* No instrument? Then we can't play. */
        }
        if (ip->samples != 1)
        {
            //ctl->cmsg(CMSG_WARNING, VERB_VERBOSE,
            //"Strange: percussion instrument with %d samples!", ip->samples);
        }
        
        if (ip->sample->note_to_use) /* Do we have a fixed pitch? */
        voice[i].orig_frequency=freq_table[(int)(ip->sample->note_to_use)];
        else
        voice[i].orig_frequency=freq_table[e->a & 0x7F];
        
        /* drums are supposed to have only one sample */
        voice[i].sample=ip->sample;
    }
    else
    {
        if (channel[e->channel].program==SPECIAL_PROGRAM)
        ip=default_instrument;
        else if (!(ip=tonebank[channel[e->channel].bank]->
        tone[channel[e->channel].program].instrument))
        {
            if (!(ip=tonebank[0]->tone[channel[e->channel].program].instrument))
            return; /* No instrument? Then we can't play. */
        }
        
        if (ip->sample->note_to_use) /* Fixed-pitch instrument? */
        voice[i].orig_frequency=freq_table[(int)(ip->sample->note_to_use)];
        else
        voice[i].orig_frequency=freq_table[e->a & 0x7F];
        select_sample(i, ip);
    }
    
    voice[i].status=VOICE_ON;
    voice[i].channel=e->channel;
    voice[i].note=e->a;
    voice[i].velocity=e->b;
    voice[i].sample_offset=0;
    voice[i].sample_increment=0; /* make sure it isn't negative */
    
    voice[i].tremolo_phase=0;
    voice[i].tremolo_phase_increment=voice[i].sample->tremolo_phase_increment;
    voice[i].tremolo_sweep=voice[i].sample->tremolo_sweep_increment;
    voice[i].tremolo_sweep_position=0;
    
    voice[i].vibrato_sweep=voice[i].sample->vibrato_sweep_increment;
    voice[i].vibrato_sweep_position=0;
    voice[i].vibrato_control_ratio=voice[i].sample->vibrato_control_ratio;
    voice[i].vibrato_control_counter=voice[i].vibrato_phase=0;
    for (j=0; j<VIBRATO_SAMPLE_INCREMENTS; j++)
    voice[i].vibrato_sample_increment[j]=0;
    
    if (channel[e->channel].panning != NO_PANNING)
    voice[i].panning=channel[e->channel].panning;
    else
    voice[i].panning=voice[i].sample->panning;
    
    recompute_freq(i);
    recompute_amp(i);
    if (voice[i].sample->modes & MODES_ENVELOPE)
    {
        /* Ramp up from 0 */
        voice[i].envelope_stage=0;
        voice[i].envelope_volume=0;
        voice[i].control_counter=0;
        recompute_envelope(i);
        apply_envelope_to_amp(i);
    }
    else
    {
        voice[i].envelope_increment=0;
        apply_envelope_to_amp(i);
    }
    //ctl->note(i);
}

static void kill_note(int i)
{
    voice[i].status=VOICE_DIE;
    //ctl->note(i);
}

/* Only one instance of a note can be playing on a single channel. */
static void note_on(MidiEvent *e)
{
    int i=voices, lowest=-1;
    int32 lv=0x7FFFFFFF, v;
    
    while (i--)
    {
        if (voice[i].status == VOICE_FREE)
        lowest=i; /* Can't get a lower volume than silence */
        else if (voice[i].channel==e->channel &&
        (voice[i].note==e->a || channel[voice[i].channel].mono))
        kill_note(i);
    }
    
    if (lowest != -1)
    {
        /* Found a free voice. */
        start_note(e,lowest);
        return;
    }
    
    /* Look for the decaying note with the lowest volume */
    i=voices;
    while (i--)
    {
        if ((voice[i].status!=VOICE_ON) &&
        (voice[i].status!=VOICE_DIE))
        {
            v=voice[i].left_mix;
            if ((voice[i].panned==PANNED_MYSTERY) && (voice[i].right_mix>v))
            v=voice[i].right_mix;
            if (v<lv)
            {
                lv=v;
                lowest=i;
            }
        }
    }
    
    if (lowest != -1)
    {
        /* This can still cause a click, but if we had a free voice to
        spare for ramping down this note, we wouldn't need to kill it
        in the first place... Still, this needs to be fixed. Perhaps
        we could use a reserve of voices to play dying notes only. */
        
        cut_notes++;
        voice[lowest].status=VOICE_FREE;
        //ctl->note(lowest);
        start_note(e,lowest);
    }
    else
    lost_notes++;
}

static void finish_note(int i)
{
    if (voice[i].sample->modes & MODES_ENVELOPE)
    {
        /* We need to get the envelope out of Sustain stage */
        voice[i].envelope_stage=3;
        voice[i].status=VOICE_OFF;
        recompute_envelope(i);
        apply_envelope_to_amp(i);
        //ctl->note(i);
    }
    else
    {
        /* Set status to OFF so resample_voice() will let this voice out
        of its loop, if any. In any case, this voice dies when it
        hits the end of its data (ofs>=data_length). */
        voice[i].status=VOICE_OFF;
    }
}

static void note_off(MidiEvent *e)
{
    int i=voices;
    while (i--)
    if (voice[i].status==VOICE_ON &&
    voice[i].channel==e->channel &&
    voice[i].note==e->a)
    {
        if (channel[e->channel].sustain)
        {
            voice[i].status=VOICE_SUSTAINED;
            //ctl->note(i);
        }
        else
        finish_note(i);
        return;
    }
}

/* Process the All Notes Off event */
static void all_notes_off(int c)
{
    int i=voices;
    //ctl->cmsg(CMSG_INFO, VERB_DEBUG, "All notes off on channel %d", c);
    while (i--)
    if (voice[i].status==VOICE_ON &&
    voice[i].channel==c)
    {
        if (channel[c].sustain)
        {
            voice[i].status=VOICE_SUSTAINED;
            //ctl->note(i);
        }
        else
        finish_note(i);
    }
}

/* Process the All Sounds Off event */
static void all_sounds_off(int c)
{
    int i=voices;
    while (i--)
    if (voice[i].channel==c &&
    voice[i].status != VOICE_FREE &&
    voice[i].status != VOICE_DIE)
    {
        kill_note(i);
    }
}

static void adjust_pressure(MidiEvent *e)
{
    int i=voices;
    while (i--)
    if (voice[i].status==VOICE_ON &&
    voice[i].channel==e->channel &&
    voice[i].note==e->a)
    {
        voice[i].velocity=e->b;
        recompute_amp(i);
        apply_envelope_to_amp(i);
        return;
    }
}

static void adjust_panning(int c)
{
    int i=voices;
    while (i--)
    if ((voice[i].channel==c) &&
    (voice[i].status==VOICE_ON || voice[i].status==VOICE_SUSTAINED))
    {
        voice[i].panning=channel[c].panning;
        recompute_amp(i);
        apply_envelope_to_amp(i);
    }
}

static void drop_sustain(int c)
{
    int i=voices;
    while (i--)
    if (voice[i].status==VOICE_SUSTAINED && voice[i].channel==c)
    finish_note(i);
}

static void adjust_pitchbend(int c)
{
    int i=voices;
    while (i--)
    if (voice[i].status!=VOICE_FREE && voice[i].channel==c)
    {
        recompute_freq(i);
    }
}

static void adjust_volume(int c)
{
    int i=voices;
    while (i--)
    if (voice[i].channel==c &&
    (voice[i].status==VOICE_ON || voice[i].status==VOICE_SUSTAINED))
    {
        recompute_amp(i);
        apply_envelope_to_amp(i);
    }
}

static void seek_forward(int32 until_time)
{
    reset_voices();
    while (current_event->time < until_time)
    {
        switch(current_event->type)
        {
            /* All notes stay off. Just handle the parameter changes. */
            
        case ME_PITCH_SENS:
            channel[current_event->channel].pitchsens=
            current_event->a;
            channel[current_event->channel].pitchfactor=0;
            break;
            
        case ME_PITCHWHEEL:
            channel[current_event->channel].pitchbend=
            current_event->a + current_event->b * 128;
            channel[current_event->channel].pitchfactor=0;
            break;
            
        case ME_MAINVOLUME:
            channel[current_event->channel].volume=current_event->a;
            break;
            
        case ME_PAN:
            channel[current_event->channel].panning=current_event->a;
            break;
            
        case ME_EXPRESSION:
            channel[current_event->channel].expression=current_event->a;
            break;
            
        case ME_PROGRAM:
            if (ISDRUMCHANNEL(current_event->channel))
            /* Change drum set */
            channel[current_event->channel].bank=current_event->a;
            else
            channel[current_event->channel].program=current_event->a;
            break;
            
        case ME_SUSTAIN:
            channel[current_event->channel].sustain=current_event->a;
            break;
            
        case ME_RESET_CONTROLLERS:
            reset_controllers(current_event->channel);
            break;
            
        case ME_TONE_BANK:
            channel[current_event->channel].bank=current_event->a;
            break;
            
        case ME_EOT:
            current_sample=current_event->time;
            return;
        }
        current_event++;
    }
    /*current_sample=current_event->time;*/
    if (current_event != event_list)
    current_event--;
    current_sample=until_time;
}

static void skip_to(int32 until_time)
{
    if (current_sample > until_time)
    current_sample=0;
    
    reset_midi();
    buffered_count=0;
	compute_sample = 0;
    buffer_pointer=common_buffer;
    current_event=event_list;
    
    if (until_time)
    seek_forward(until_time);
}

static void do_compute_data(int32 count)
{
    int i;
    memset(buffer_pointer, 0,
    (play_mode->encoding & PE_MONO) ? (count * 4) : (count * 8));
    for (i=0; i<voices; i++)
    {
        if(voice[i].status != VOICE_FREE)
			mix_voice(buffer_pointer, i, count);
    }
    current_sample += count;
}

/* count=0 means flush remaining buffered data to output device, then
flush the device itself */
static int compute_data(int32 count)
{
    if (!count)
    {
        if (buffered_count)
        play_mode->output_data(common_buffer, buffered_count);
        play_mode->flush_output();
        buffer_pointer=common_buffer;
        buffered_count=0;
        return 0;
    }
    
    while ((count+buffered_count) >= AUDIO_BUFFER_SIZE)
    {
        do_compute_data(AUDIO_BUFFER_SIZE-buffered_count);
        count -= AUDIO_BUFFER_SIZE-buffered_count;
        play_mode->output_data(common_buffer, AUDIO_BUFFER_SIZE);
        buffer_pointer=common_buffer;
        buffered_count=0;
    }
    if (count>0)
    {
        do_compute_data(count);
        buffered_count += count;
        buffer_pointer += (play_mode->encoding & PE_MONO) ? count : count*2;
    }
    return 0;
}

FILE* fp_midi = NULL;
int32 events_midi = 0;

int open_midi_file(char* fn, int* duration, int* bitrate)
{
	fp_midi = open_file(fn, 1, OF_VERBOSE);
	if (!fp_midi)
		return 0;
    
    event_list = read_midi_file(fp_midi, &events_midi, &sample_count);
	if (!event_list || !events_midi || !sample_count) {
		fclose(fp_midi);
		return 0;
	}

	skip_to(0);

	*duration = (int)((double)sample_count * 1000 / play_mode->rate);

	fseek(fp_midi, 0, SEEK_END);
	*bitrate = ftell(fp_midi) * 8 / (*duration);
	fseek(fp_midi, 0, SEEK_SET);
	return 1;
}

int start_play_midi()
{
	start_load_missing_instruments();

	adjust_amplification();    
    lost_notes = cut_notes = 0;

	return 1;
}

int play_midi(int maxlen)
{
	int convert;

	if (load_missing_instruments())
		return 2;

	if (compute_sample) {
		if (maxlen > compute_sample) {
			compute_data(compute_sample);
			compute_sample = 0;
		}
		else {
			compute_data(maxlen);
			compute_sample -= maxlen;
		}
		return 0;
	}

	/* Handle all events that should happen at this time */
    while (current_event->time <= current_sample)
    {
        switch(current_event->type)
        {
            
            /* Effects affecting a single note */
            
        case ME_NOTEON:
            if (!(current_event->b)) /* Velocity 0? */
            note_off(current_event);
            else
            note_on(current_event);
            break;
            
        case ME_NOTEOFF:
            note_off(current_event);
            break;
            
        case ME_KEYPRESSURE:
            adjust_pressure(current_event);
            break;
            
            /* Effects affecting a single channel */
            
        case ME_PITCH_SENS:
            channel[current_event->channel].pitchsens=
            current_event->a;
            channel[current_event->channel].pitchfactor=0;
            break;
            
        case ME_PITCHWHEEL:
            channel[current_event->channel].pitchbend=
            current_event->a + current_event->b * 128;
            channel[current_event->channel].pitchfactor=0;
            /* Adjust pitch for notes already playing */
            adjust_pitchbend(current_event->channel);
            //ctl->pitch_bend(current_event->channel,
            //channel[current_event->channel].pitchbend);
            break;
            
        case ME_MAINVOLUME:
            channel[current_event->channel].volume=current_event->a;
            adjust_volume(current_event->channel);
            //ctl->volume(current_event->channel, current_event->a);
            break;
            
        case ME_PAN:
            channel[current_event->channel].panning=current_event->a;
            if (adjust_panning_immediately)
            adjust_panning(current_event->channel);
            //ctl->panning(current_event->channel, current_event->a);
            break;
            
        case ME_EXPRESSION:
            channel[current_event->channel].expression=current_event->a;
            adjust_volume(current_event->channel);
            //ctl->expression(current_event->channel, current_event->a);
            break;
            
        case ME_PROGRAM:
            if (ISDRUMCHANNEL(current_event->channel))
            {
                /* Change drum set */
                channel[current_event->channel].bank=current_event->a;
            }
            else
            {
                channel[current_event->channel].program=current_event->a;
            }
            //ctl->program(current_event->channel, current_event->a);
            break;
            
        case ME_SUSTAIN:
            channel[current_event->channel].sustain=current_event->a;
            if (!current_event->a)
            drop_sustain(current_event->channel);
            //ctl->sustain(current_event->channel, current_event->a);
            break;
            
        case ME_RESET_CONTROLLERS:
            reset_controllers(current_event->channel);
            redraw_controllers(current_event->channel);
            break;
            
        case ME_ALL_NOTES_OFF:
            all_notes_off(current_event->channel);
            break;
            
        case ME_ALL_SOUNDS_OFF:
            all_sounds_off(current_event->channel);
            break;
            
        case ME_TONE_BANK:
            channel[current_event->channel].bank=current_event->a;
            break;
            
        case ME_EOT:
			return 1;
        }
        current_event++;
    }
    
	convert = current_event->time - current_sample;
	if (maxlen >= convert)
		compute_data(convert);
	else {
		compute_data(maxlen);
		compute_sample = convert - maxlen;
	}
   
	return 0;
}

long seek_midi(long time)
{
	skip_to(0);
	skip_to((int32)((double)play_mode->rate * time / 1000));
	return (long)((double)current_sample * 1000 / play_mode->rate);;
}

void stop_play_midi()
{
    lost_notes = cut_notes = 0;
}

void close_midi()
{
	if (free_instruments_afterwards)
		free_instruments();

	if (event_list) {
		free(event_list);
		event_list = NULL;
	}
	if (fp_midi) {
		close_file(fp_midi);
		fp_midi = NULL;
	}
	events_midi = 0;
	sample_count = 0;
}