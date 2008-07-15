#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef _WIN32_WCE
#include <mmsystem.h>
#endif

#include "mapplugin.h"
#include "in_mp4.h"

extern "C" {
#include "utils.h"
}

BOOL GetId3TagV1(FILE* fp, MAP_PLUGIN_FILETAG* pTag);
BOOL GetId3TagV2(FILE* fp, MAP_PLUGIN_FILETAG* pTag);

state mp4state = {0};
uint8_t* leftoutbuf = NULL;
int leftoutbuflen = 0;

void flushleftbuf()
{
	if (leftoutbuf) {
		free(leftoutbuf);
		leftoutbuf = NULL;
	}
	leftoutbuf = 0;
}

uint32_t read_callback(void *user_data, void *buffer, uint32_t length)
{
    return fread(buffer, 1, length, (FILE*)user_data);
}

uint32_t seek_callback(void *user_data, uint64_t position)
{
    return fseek((FILE*)user_data, (uint32_t)position, SEEK_SET);
}

uint32_t write_callback(void *user_data, void *buffer, uint32_t length)
{
    return fwrite(buffer, 1, length, (FILE*)user_data);
}

uint32_t truncate_callback(void *user_data)
{
    //_chsize(fileno((FILE*)user_data), ftell((FILE*)user_data));
    //return 1;

	// update tag future is not supported!
	return 0;
}

int skip_id3v2_tag()
{
    unsigned char buf[10];
    int bread, tagsize = 0;

    bread = fread(buf, 1, 10, mp4state.aacfile);
    if (bread != 10) return -1;

    if (!memcmp(buf, "ID3", 3))
    {
        /* high bit is not used */
        tagsize = (buf[6] << 21) | (buf[7] << 14) | (buf[8] << 7) | (buf[9] << 0);

        tagsize += 10;
        fseek(mp4state.aacfile, tagsize, SEEK_SET);
    } else {
        fseek(mp4state.aacfile, 0, SEEK_SET);
    }

    return tagsize;
}

int fill_buffer(state *st)
{
    int bread;

    if (st->m_aac_bytes_consumed > 0)
    {
        if (st->m_aac_bytes_into_buffer)
        {
            memmove((void*)st->m_aac_buffer, (void*)(st->m_aac_buffer + st->m_aac_bytes_consumed),
                st->m_aac_bytes_into_buffer*sizeof(unsigned char));
        }

        if (!st->m_at_eof)
        {
            bread = fread((void*)(st->m_aac_buffer + st->m_aac_bytes_into_buffer),
                1, st->m_aac_bytes_consumed, st->aacfile);

            if (bread != st->m_aac_bytes_consumed)
                st->m_at_eof = 1;

            st->m_aac_bytes_into_buffer += bread;
        }

        st->m_aac_bytes_consumed = 0;

        if (st->m_aac_bytes_into_buffer > 3)
        {
            if (memcmp(st->m_aac_buffer, "TAG", 3) == 0)
                st->m_aac_bytes_into_buffer = 0;
        }
        if (st->m_aac_bytes_into_buffer > 11)
        {
            if (memcmp(st->m_aac_buffer, "LYRICSBEGIN", 11) == 0)
                st->m_aac_bytes_into_buffer = 0;
        }
        if (st->m_aac_bytes_into_buffer > 8)
        {
            if (memcmp(st->m_aac_buffer, "APETAGEX", 8) == 0)
                st->m_aac_bytes_into_buffer = 0;
        }
    }

    return 1;
}

void advance_buffer(state *st, int bytes)
{
    st->m_file_offset += bytes;
    st->m_aac_bytes_consumed = bytes;
    st->m_aac_bytes_into_buffer -= bytes;
}

int file_length(FILE *f)
{
    long end = 0;
    long cur = ftell(f);
    fseek(f, 0, SEEK_END);
    end = ftell(f);
    fseek(f, cur, SEEK_SET);

    return end;
}

int is_adts_header(unsigned char* buf, int length)
{
	int frame_length;
	if (length < 8)
		return 0;

	// check syncword 
	if (!((buf[0] == 0xFF)&&((buf[1] & 0xF6) == 0xF0)))
		return 0;

	// check samplerate
	if ((buf[2]&0x3c)>>2 > 0x11)
		return 0;

	return 1;
}

int is_adif_header(unsigned char* buf, int length)
{
	if (buf[0] == 'A' && buf[1] == 'D' && buf[2] == 'I' && buf[3] == 'F')
		return 1;

	return 0;
}

int is_aac_frame_header(unsigned char* buf, int length)
{
	if (is_adts_header(buf, length))
		return 1;

	if (is_adif_header(buf, length))
		return 1;

	return 0;
}

int adts_parse(state *st, __int64 *bitrate, double *length)
{
    static int sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000};
    int frames, frame_length;
    int t_framelength = 0;
    int samplerate;
    double frames_per_sec, bytes_per_frame;
	double filelength = (double)file_length(mp4state.aacfile);

#define MAX_AAC_PARSE_HEADER	128

    /* Read all frames to ensure correct time and bitrate */
    for (frames = 0; frames < MAX_AAC_PARSE_HEADER; frames++)
    {
		fill_buffer(st);

		if (st->m_aac_bytes_into_buffer > 7)
        {
            /* check syncword */
            if (!((st->m_aac_buffer[0] == 0xFF)&&((st->m_aac_buffer[1] & 0xF6) == 0xF0)))
                break;

			if (frames == 0)
                samplerate = sample_rates[(st->m_aac_buffer[2]&0x3c)>>2];

            frame_length = ((((unsigned int)st->m_aac_buffer[3] & 0x3)) << 11)
                | (((unsigned int)st->m_aac_buffer[4]) << 3) | (st->m_aac_buffer[5] >> 5);

            t_framelength += frame_length;

            if (frame_length > st->m_aac_bytes_into_buffer)
                break;

            advance_buffer(st, frame_length);
        } else {
            break;
        }
	}

	frames_per_sec = (double)samplerate/1024.0;
    if (frames != 0)
        bytes_per_frame = (double)t_framelength/(double)(frames*1000);
    else
        bytes_per_frame = 0;
    *bitrate = (__int64)(8. * bytes_per_frame * frames_per_sec);
    if (frames_per_sec != 0)
		*length = filelength / ((8. * bytes_per_frame * frames_per_sec) * 1000 / 8);
    else
        *length = 1;

	return 1;
}

void *decode_aac_frame(state *st, NeAACDecFrameInfo *frameInfo)
{
    void *sample_buffer = NULL;

    do
    {
        fill_buffer(st);

        if (st->m_aac_bytes_into_buffer != 0)
        {
            sample_buffer = NeAACDecDecode(st->hDecoder, frameInfo,
                st->m_aac_buffer, st->m_aac_bytes_into_buffer);

            if (st->m_header_type != 1)
            {
                if (st->last_offset < st->m_file_offset)
                {
                    st->m_tail->offset = st->m_file_offset;
                    st->m_tail->next = (struct seek_list*)malloc(sizeof(struct seek_list));
                    st->m_tail = st->m_tail->next;
                    st->m_tail->next = NULL;
                    st->last_offset = st->m_file_offset;
                }
            }

            advance_buffer(st, frameInfo->bytesconsumed);
        } else {
            break;
        }

    } while (!frameInfo->samples && !frameInfo->error);

    return sample_buffer;
}

int aac_seek(state *st, double seconds)
{
	double filelength = file_length(st->aacfile);
	double offset;
	unsigned long samplerate;
	unsigned char channels;
	NeAACDecFrameInfo frameInfo = {0};

	//seconds = 63.073000000000000;

	offset = (filelength - st->m_tagsize) / (st->m_length / 1000) * seconds;
	offset += st->m_tagsize;

	fseek(st->aacfile, offset, SEEK_SET);
	st->m_file_offset = offset;
	st->m_at_eof = 0;
	st->m_aac_bytes_consumed = 768*6;
	st->m_aac_bytes_into_buffer = 0;
	fill_buffer(st);

	NeAACDecPostSeekReset(st->hDecoder, -1);

	while (!st->m_at_eof && st->m_aac_bytes_into_buffer) {
		if (is_aac_frame_header(st->m_aac_buffer, st->m_aac_bytes_into_buffer)) {
			memset(&frameInfo, 0, sizeof(frameInfo));
			NeAACDecDecode(mp4state.hDecoder, &frameInfo, mp4state.m_aac_buffer, mp4state.m_aac_bytes_into_buffer);
			if (frameInfo.error == 0) {
				break;
			}
		}

		advance_buffer(st, 1);
		fill_buffer(st);
	}
	
	if (st->m_at_eof || !st->m_aac_bytes_into_buffer)
		return 0;

	NeAACDecPostSeekReset(st->hDecoder, -1);
	return 1;
}

int isourfile(TCHAR *fn)
{
	if (!_tcsicmp(fn + _tcslen(fn) - 4, _T(".MP4")))
		return 1;
	if (!_tcsicmp(fn + _tcslen(fn) - 4, _T(".M4A")))
		return 1;
	if (!_tcsicmp(fn + _tcslen(fn) - 4, _T(".AAC")))
		return 1;
	if (!_tcsicmp(fn + _tcslen(fn) - 4, _T(".3GP")))
		return 1;
	
	return 0;
}

static void remap_channels(unsigned char *data, unsigned int samples, unsigned int bps)
{
    unsigned int i;

    switch (bps)
    {
    case 8:
        {
            unsigned char r1, r2, r3, r4, r5, r6;
            for (i = 0; i < samples; i += 6)
            {
                r1 = data[i];
                r2 = data[i+1];
                r3 = data[i+2];
                r4 = data[i+3];
                r5 = data[i+4];
                r6 = data[i+5];
                data[i] = r2;
                data[i+1] = r3;
                data[i+2] = r1;
                data[i+3] = r6;
                data[i+4] = r4;
                data[i+5] = r5;
            }
        }
        break;

    case 16:
    default:
        {
            unsigned short r1, r2, r3, r4, r5, r6;
            unsigned short *sample_buffer = (unsigned short *)data;
            for (i = 0; i < samples; i += 6)
            {
                r1 = sample_buffer[i];
                r2 = sample_buffer[i+1];
                r3 = sample_buffer[i+2];
                r4 = sample_buffer[i+3];
                r5 = sample_buffer[i+4];
                r6 = sample_buffer[i+5];
                sample_buffer[i] = r2;
                sample_buffer[i+1] = r3;
                sample_buffer[i+2] = r1;
                sample_buffer[i+3] = r6;
                sample_buffer[i+4] = r4;
                sample_buffer[i+5] = r5;
            }
        }
        break;

    case 24:
    case 32:
        {
            unsigned int r1, r2, r3, r4, r5, r6;
            unsigned int *sample_buffer = (unsigned int *)data;
            for (i = 0; i < samples; i += 6)
            {
                r1 = sample_buffer[i];
                r2 = sample_buffer[i+1];
                r3 = sample_buffer[i+2];
                r4 = sample_buffer[i+3];
                r5 = sample_buffer[i+4];
                r6 = sample_buffer[i+5];
                sample_buffer[i] = r2;
                sample_buffer[i+1] = r3;
                sample_buffer[i+2] = r1;
                sample_buffer[i+3] = r6;
                sample_buffer[i+4] = r4;
                sample_buffer[i+5] = r5;
            }
        }
        break;
    }
}

int mp4open(TCHAR *fn)
{
    unsigned char *buffer;
    int buffer_size;
    NeAACDecConfigurationPtr config;
    unsigned char header[8];
    FILE *hMP4File;

    memset(&mp4state, 0, sizeof(state));
    lstrcpy(mp4state.filename, fn);

    hMP4File = _tfopen(mp4state.filename, _T("rb"));
    if (!hMP4File)
    {
        return -1;
    }
    fread(header, 1, 8, hMP4File);
    fclose(hMP4File);

    if (header[4] != 'f' || header[5] != 't' || header[6] != 'y' || header[7] != 'p')
        return -1;

    mp4state.hDecoder = NeAACDecOpen();
    if (!mp4state.hDecoder)
    {
        return -1;
    }

    config = NeAACDecGetCurrentConfiguration(mp4state.hDecoder);
    config->outputFormat = FAAD_FMT_16BIT;
    config->downMatrix = 1;
    NeAACDecSetConfiguration(mp4state.hDecoder, config);

    mp4state.mp4File = _tfopen(mp4state.filename, _T("rb"));
    mp4state.mp4cb.read = read_callback;
    mp4state.mp4cb.seek = seek_callback;
    mp4state.mp4cb.user_data = mp4state.mp4File;


    mp4state.mp4file = mp4ff_open_read(&mp4state.mp4cb);
    if (!mp4state.mp4file)
    {
        close();
        return -1;
    }

    if ((mp4state.mp4track = GetAACTrack(mp4state.mp4file)) < 0)
    {
        close();
        return -1;
    }

    buffer = NULL;
    buffer_size = 0;
    mp4ff_get_decoder_config(mp4state.mp4file, mp4state.mp4track,
        &buffer, (unsigned int*)&buffer_size);
    if (!buffer)
    {
        close();
        return -1;
    }

    if(NeAACDecInit2(mp4state.hDecoder, buffer, buffer_size,
        (unsigned long*)&mp4state.samplerate, &mp4state.channels) < 0)
    {
        /* If some error initializing occured, skip the file */
        if (buffer) free (buffer);
		close();
        return -1;
    }

    /* for gapless decoding */
    {
        mp4AudioSpecificConfig mp4ASC;

        mp4state.timescale = mp4ff_time_scale(mp4state.mp4file, mp4state.mp4track);
        mp4state.framesize = 1024;
        mp4state.useAacLength = 0;

        if (buffer)
        {
            if (NeAACDecAudioSpecificConfig(buffer, buffer_size, &mp4ASC) >= 0)
            {
                if (mp4ASC.frameLengthFlag == 1) mp4state.framesize = 960;
                if (mp4ASC.sbr_present_flag == 1) mp4state.framesize *= 2;
            }
        }
    }

    free(buffer);

    mp4state.avg_bitrate = mp4ff_get_avg_bitrate(mp4state.mp4file, mp4state.mp4track);

    mp4state.numSamples = mp4ff_num_samples(mp4state.mp4file, mp4state.mp4track);
    mp4state.sampleId = 0;

    {
        double timescale_div = 1.0 / (double)mp4ff_time_scale(mp4state.mp4file, mp4state.mp4track);
        int64_t duration = mp4ff_get_track_duration_use_offsets(mp4state.mp4file, mp4state.mp4track);
        if (duration == -1)
        {
            mp4state.m_length = 0;
        } else {
            mp4state.m_length = (int)((double)duration * timescale_div * 1000.0);
        }
    }

    mp4state.is_seekable = 1;

    if (mp4state.channels == 0)
    {
        close();
        return -1;
    }

    if (/*m_downmix && */(mp4state.channels == 5 || mp4state.channels == 6))
        mp4state.channels = 2;

    mp4state.paused        =  0;
    mp4state.decode_pos_ms =  0;
    mp4state.seek_needed   = -1;

	mp4state.filetype = 0;
	NeAACDecPostSeekReset(mp4state.hDecoder, -1);
	return 0;
}

int aacopen(TCHAR *fn)
{
    unsigned char *buffer;
    int buffer_size;
    NeAACDecConfigurationPtr config;
    unsigned char header[8];
    FILE *hMP4File;
	int tmp = 0, init;
    int bread = 0;
    double length = 0.;
    __int64 bitrate = 128;

    memset(&mp4state, 0, sizeof(state));
    lstrcpy(mp4state.filename, fn);

    mp4state.is_seekable = 1;

    if (!(mp4state.aacfile = _tfopen(mp4state.filename, _T("rb"))))
    {
		close();   
        return -1;
    }

    mp4state.m_tagsize = skip_id3v2_tag();
	if (mp4state.m_tagsize<0)
	{
		close();
		return 0;
	}

    if (!(mp4state.m_aac_buffer = (unsigned char*)malloc(768*6)))
    {
		close();
        return -1;
    }

    for (init=0; init<2; init++)
    {
        mp4state.hDecoder = NeAACDecOpen();
        if (!mp4state.hDecoder)
        {
			close();
            return -1;
        }

        config = NeAACDecGetCurrentConfiguration(mp4state.hDecoder);
        config->outputFormat = FAAD_FMT_16BIT;
        config->downMatrix = 1;
        NeAACDecSetConfiguration(mp4state.hDecoder, config);

        memset(mp4state.m_aac_buffer, 0, 768*6);
        bread = fread(mp4state.m_aac_buffer, 1, 768*6, mp4state.aacfile);
        mp4state.m_aac_bytes_into_buffer = bread;
        mp4state.m_aac_bytes_consumed = 0;
        mp4state.m_file_offset = 0;
        mp4state.m_at_eof = (bread != 768*6) ? 1 : 0;

        if (init==0)
        {
            NeAACDecFrameInfo frameInfo;

            fill_buffer(&mp4state);

            if ((mp4state.m_aac_bytes_consumed = NeAACDecInit(mp4state.hDecoder,
                mp4state.m_aac_buffer, mp4state.m_aac_bytes_into_buffer,
                (unsigned long*)&mp4state.samplerate, &mp4state.channels)) < 0)
            {
				close();
                return -1;
            }
            advance_buffer(&mp4state, mp4state.m_aac_bytes_consumed);

            do {
                memset(&frameInfo, 0, sizeof(NeAACDecFrameInfo));
                fill_buffer(&mp4state);
                NeAACDecDecode(mp4state.hDecoder, &frameInfo, mp4state.m_aac_buffer, mp4state.m_aac_bytes_into_buffer);
            } while (!frameInfo.samples && !frameInfo.error);

            if (frameInfo.error)
            {
				close();
                return -1;
            }

            mp4state.channels = frameInfo.channels;
            mp4state.samplerate = frameInfo.samplerate;
            mp4state.framesize = (frameInfo.channels != 0) ? frameInfo.samples/frameInfo.channels : 0;
            /*
            sbr = frameInfo.sbr;
            profile = frameInfo.object_type;
            header_type = frameInfo.header_type;
            */

            NeAACDecClose(mp4state.hDecoder);
            fseek(mp4state.aacfile, mp4state.m_tagsize, SEEK_SET);
        }
    }

    mp4state.m_head = (struct seek_list*)malloc(sizeof(struct seek_list));
    mp4state.m_tail = mp4state.m_head;
    mp4state.m_tail->next = NULL;

    mp4state.m_header_type = 0;
    if ((mp4state.m_aac_buffer[0] == 0xFF) && ((mp4state.m_aac_buffer[1] & 0xF6) == 0xF0))
    {
        if (1) //(can_seek)
        {
            adts_parse(&mp4state, &bitrate, &length);
            fseek(mp4state.aacfile, mp4state.m_tagsize, SEEK_SET);

            bread = fread(mp4state.m_aac_buffer, 1, 768*6, mp4state.aacfile);
            if (bread != 768*6)
                mp4state.m_at_eof = 1;
            else
                mp4state.m_at_eof = 0;
            mp4state.m_aac_bytes_into_buffer = bread;
            mp4state.m_aac_bytes_consumed = 0;

            mp4state.m_header_type = 1;
        }
    } else if (memcmp(mp4state.m_aac_buffer, "ADIF", 4) == 0) {
        int skip_size = (mp4state.m_aac_buffer[4] & 0x80) ? 9 : 0;
        bitrate = ((unsigned int)(mp4state.m_aac_buffer[4 + skip_size] & 0x0F)<<19) |
            ((unsigned int)mp4state.m_aac_buffer[5 + skip_size]<<11) |
            ((unsigned int)mp4state.m_aac_buffer[6 + skip_size]<<3) |
            ((unsigned int)mp4state.m_aac_buffer[7 + skip_size] & 0xE0);

        length = (double)file_length(mp4state.aacfile);
        if (length == -1)
        {
            mp4state.is_seekable = 0;
            length = 0;
        } else {
            length = ((double)length*8.)/((double)bitrate) + 0.5;
        }

        mp4state.m_header_type = 2;
    } else {
        length = (double)file_length(mp4state.aacfile);
        length = ((double)length*8.)/((double)bitrate*1000.) + 0.5;

        mp4state.is_seekable = 1;
    }

    mp4state.m_length = (int)(length*1000.);

    fill_buffer(&mp4state);
    if ((mp4state.m_aac_bytes_consumed = NeAACDecInit(mp4state.hDecoder,
        mp4state.m_aac_buffer, mp4state.m_aac_bytes_into_buffer,
        (unsigned long*)&mp4state.samplerate, &mp4state.channels)) < 0)
    {
		close();
        return -1;
    }
    advance_buffer(&mp4state, mp4state.m_aac_bytes_consumed);

    if (mp4state.m_header_type == 2)
        mp4state.avg_bitrate = bitrate;
    else
        mp4state.avg_bitrate = bitrate*1000;

	if (/*m_downmix && */(mp4state.channels == 5 || mp4state.channels == 6))
        mp4state.channels = 2;

	mp4state.filetype = 1;
	NeAACDecPostSeekReset(mp4state.hDecoder, -1);
	return 0;
}

int open(TCHAR *fn)
{
	if (!mp4open(fn))
		return 0;
	if (!aacopen(fn))
		return 0;

	return -1;
}

void close()
{
	struct seek_list *target = mp4state.m_head;
	
	if (mp4state.m_aac_buffer)
        free(mp4state.m_aac_buffer);

    while (target)
    {
        struct seek_list *tmp = target;
        target = target->next;
        if (tmp) free(tmp);
    }
	if (mp4state.hDecoder) {
		NeAACDecClose(mp4state.hDecoder);
	}

	if (mp4state.aacfile) {
		fclose(mp4state.aacfile);
	}

	if (mp4state.mp4file) {
		mp4ff_close(mp4state.mp4file);
	}
	if (mp4state.mp4File) {
		fclose(mp4state.mp4File);
	}

	flushleftbuf();

    memset(&mp4state, 0, sizeof(state));
}

int mp4setoutputtime(int time_in_ms)
{
	int64_t duration;
	int32_t skip_samples = 0;

	if (!mp4state.mp4File)
		return -1;

	if (!mp4state.is_seekable)
		return -1;

    duration = (int64_t)(time_in_ms/1000.0 * mp4state.samplerate + 0.5);
	mp4state.sampleId = mp4ff_find_sample_use_offsets(mp4state.mp4file, mp4state.mp4track, duration, &skip_samples);
	mp4state.decode_pos_ms = time_in_ms;

	flushleftbuf();

	return time_in_ms;
}

int aacsetoutputtime(int time_in_ms)
{
	double ms;

	if (!mp4state.aacfile)
		return -1;

    ms = (double)time_in_ms/1000;
    if (aac_seek(&mp4state, ms)!=0)
    {
        mp4state.cur_pos_sec = ms;
        mp4state.decode_pos_ms = time_in_ms;

		flushleftbuf();

		return time_in_ms;
    }
	return -1;
}

int setoutputtime(int time_in_ms)
{
	if (mp4state.filetype)
		return aacsetoutputtime(time_in_ms);
	else
		return mp4setoutputtime(time_in_ms);
}

int start()
{
	if (!mp4state.mp4File && !mp4state.aacfile)
		return 0;

	mp4state.last_frame = 0;
    mp4state.initial = 1;

	return 1;
}

void stop()
{
	flushleftbuf();
}

int mp4decode(uint8_t *outbuf, uint32_t outbuflen, uint32_t *outlen)
{
	unsigned char *buffer = NULL;
    int buffer_size = 0;
	NeAACDecFrameInfo frameInfo = {0};
	void *sample_buffer;
	unsigned int sample_count;
    unsigned int delay = 0;
	char *buf;

	*outlen = 0;

	if (!mp4state.mp4File)
		return PLUGIN_RET_ERROR;

	if (leftoutbuf && leftoutbuflen) {
		if (outbuflen < leftoutbuflen) {
			memcpy(outbuf, leftoutbuf, outbuflen);
			*outlen = outbuflen;

			memmove(leftoutbuf, leftoutbuf + outbuflen, leftoutbuflen - outbuflen);
			leftoutbuflen -= outbuflen;
			return PLUGIN_RET_SUCCESS;
		}
		else {
			memcpy(outbuf, leftoutbuf, leftoutbuflen);
			outbuf += leftoutbuflen;
			*outlen += leftoutbuflen;
			
			free(leftoutbuf);
			leftoutbuflen = 0;
		}
	}

	while (outbuflen - *outlen && !mp4state.last_frame) {
		buffer = NULL;
		buffer_size = 0;

		long dur = mp4ff_get_sample_duration(mp4state.mp4file, mp4state.mp4track, mp4state.sampleId);
		if (dur == -1)
			break;
		int rc = mp4ff_read_sample(mp4state.mp4file, mp4state.mp4track, mp4state.sampleId++, &buffer,  (unsigned int*)&buffer_size);
		if (rc == 0 || buffer == NULL) {
			mp4state.last_frame = 1;
			sample_buffer = NULL;
			frameInfo.samples = 0;
		} 
		else {
			sample_buffer = NeAACDecDecode(mp4state.hDecoder, &frameInfo, buffer, buffer_size);
		}
	
		if (mp4state.sampleId >= mp4state.numSamples)
			mp4state.last_frame = 1;

		if (buffer) {
			free(buffer);
		}
		if (frameInfo.error > 0) {
			return PLUGIN_RET_ERROR;
		} 
		if (frameInfo.samplerate != mp4state.samplerate) {
			return PLUGIN_RET_ERROR;
		}

		if (mp4state.useAacLength || (mp4state.timescale != mp4state.samplerate)) {
			sample_count = frameInfo.samples;
		}
		else {
			sample_count = (unsigned int)(dur * frameInfo.channels);
			if (!mp4state.useAacLength && !mp4state.initial && (mp4state.sampleId < mp4state.numSamples/2) && (dur*frameInfo.channels != frameInfo.samples)) {
				mp4state.useAacLength = 1;
				sample_count = frameInfo.samples;
			}
		}

		if (mp4state.initial && (sample_count < mp4state.framesize*mp4state.channels) && (frameInfo.samples > sample_count)) {
			delay = frameInfo.samples - sample_count;
		}

		if (sample_count > 0 && sample_buffer) {
			buf = (char *)sample_buffer;
			mp4state.initial = 0;
			buf += delay * 2;
	        
			if (frameInfo.channels == 6 && frameInfo.num_lfe_channels)
				remap_channels((unsigned char*)buf, sample_count, 16);

			int l = sample_count * 16 / 8;

			if (outbuflen - *outbuf < l) {
				memcpy(outbuf + *outlen, buf, outbuflen - *outbuf);
				leftoutbuf = (uint8_t*)malloc(l - outbuflen - *outbuf);
				leftoutbuflen = l - outbuflen - *outbuf;
				memcpy(leftoutbuf, buf + outbuflen - *outbuf, leftoutbuflen);

				*outlen = outbuflen;
			}
			else {
				memcpy(outbuf + *outlen, buf, l);
				*outlen += l;
			}
		}

		mp4state.decode_pos_ms += (double)sample_count * 1000.0 /
                        ((double)frameInfo.samplerate * (double)frameInfo.channels);
	}

	if (mp4state.last_frame && !leftoutbuflen)
		return PLUGIN_RET_EOF;

	return PLUGIN_RET_SUCCESS;
}

int aacdecode(uint8_t *outbuf, uint32_t outbuflen, uint32_t *outlen)
{
	void *sample_buffer;
	char *buf;
	NeAACDecFrameInfo frameInfo;

	*outlen = 0;

	if (!mp4state.aacfile)
		return PLUGIN_RET_ERROR;

	if (leftoutbuf && leftoutbuflen) {
		if (outbuflen < leftoutbuflen) {
			memcpy(outbuf, leftoutbuf, outbuflen);
			*outlen = outbuflen;

			memmove(leftoutbuf, leftoutbuf + outbuflen, leftoutbuflen - outbuflen);
			leftoutbuflen -= outbuflen;
			return PLUGIN_RET_SUCCESS;
		}
		else {
			memcpy(outbuf, leftoutbuf, leftoutbuflen);
			outbuf += leftoutbuflen;
			*outlen += leftoutbuflen;
			
			free(leftoutbuf);
			leftoutbuflen = 0;
		}
	}

	while (outbuflen - *outlen && !mp4state.last_frame) {
		memset(&frameInfo, 0, sizeof(NeAACDecFrameInfo));
		sample_buffer = decode_aac_frame(&mp4state, &frameInfo);
		if (mp4state.m_aac_bytes_into_buffer == 0) {
			mp4state.last_frame = 1;
			break;
		}
		if (frameInfo.error) {
			return PLUGIN_RET_ERROR;
		}
		if (frameInfo.samplerate != mp4state.samplerate) {
 			return PLUGIN_RET_ERROR;
		}

		if (frameInfo.samples > 0 && sample_buffer) {
			buf = (char *)sample_buffer;
			mp4state.initial = 0;
	        
			if (frameInfo.channels == 6 && frameInfo.num_lfe_channels)
				remap_channels((unsigned char*)buf, frameInfo.samples, 16);

			int l = frameInfo.samples * 16 / 8;

			if (outbuflen - *outbuf < l) {
				memcpy(outbuf + *outlen, buf, outbuflen - *outbuf);
				leftoutbuf = (uint8_t*)malloc(l - outbuflen - *outbuf);
				leftoutbuflen = l - outbuflen - *outbuf;
				memcpy(leftoutbuf, buf + outbuflen - *outbuf, leftoutbuflen);

				*outlen = outbuflen;
			}
			else {
				memcpy(outbuf + *outlen, buf, l);
				*outlen += l;
			}
		}

		mp4state.decode_pos_ms += (double)frameInfo.samples * 1000.0 /
			((double)frameInfo.samplerate* (double)frameInfo.channels);

		if (frameInfo.channels > 0 && mp4state.samplerate > 0)
			mp4state.cur_pos_sec += ((double)(frameInfo.samples/frameInfo.channels))/(double)mp4state.samplerate;
	}

	if (mp4state.last_frame && !leftoutbuflen)
		return PLUGIN_RET_EOF;

	return PLUGIN_RET_SUCCESS;
}

int decode(uint8_t *outbuf, uint32_t outbuflen, uint32_t *outlen)
{
	if (mp4state.filetype)
		return aacdecode(outbuf, outbuflen, outlen);
	else
		return mp4decode(outbuf, outbuflen, outlen);
}

/* Convert UTF-8 coded string to UNICODE
   Return number of characters converted */
int utf8ToUnicode( const char* lpMultiByteStr, WCHAR* lpWideCharStr, int cmbChars )
{
    const unsigned char*    pmb = (unsigned char  *)lpMultiByteStr;
    unsigned short*         pwc = (unsigned short *)lpWideCharStr;
    const unsigned char*    pmbe;
    size_t  cwChars = 0;

    if ( cmbChars >= 0 ) {
        pmbe = pmb + cmbChars;
    } else {
        pmbe = (unsigned char *)((size_t)-1);
    }

    while ( pmb < pmbe ) {
        char            mb = *pmb++;
        unsigned int    cc = 0;
        unsigned int    wc;

        while ( (cc < 7) && (mb & (1 << (7 - cc)))) {
            cc++;
        }

        if ( cc == 1 || cc > 6 )                    // illegal character combination for UTF-8
            continue;

        if ( cc == 0 ) {
            wc = mb;
        } else {
            wc = (mb & ((1 << (7 - cc)) - 1)) << ((cc - 1) * 6);
            while ( --cc > 0 ) {
                if ( pmb == pmbe )                  // reached end of the buffer
                    return cwChars;
                mb = *pmb++;
                if ( ((mb >> 6) & 0x03) != 2 )      // not part of multibyte character
                    return cwChars;
                wc |= (mb & 0x3F) << ((cc - 1) * 6);
            }
        }

        if ( wc & 0xFFFF0000 )
            wc = L'?';
        *pwc++ = wc;
        cwChars++;
        if ( wc == L'\0' )
            return cwChars;
    }

    return cwChars;
}

int mp4gettag(mp4ff_t *file, MAP_PLUGIN_FILETAG* tag)
{
	char* val;
	char year[5] = {0};

	memset(tag, 0, sizeof(MAP_PLUGIN_FILETAG));
	mp4ff_meta_get_title(file, &val);
	if (val) {
		utf8ToUnicode(val, tag->szTrack, MAX_PLUGIN_TAG_STR);
	}

	mp4ff_meta_get_artist(file, &val);
	if (val) {
		utf8ToUnicode(val, tag->szArtist, MAX_PLUGIN_TAG_STR);
	}

	mp4ff_meta_get_album(file, &val);
	if (val) {
		utf8ToUnicode(val, tag->szAlbum, MAX_PLUGIN_TAG_STR);
	}

	mp4ff_meta_get_comment(file, &val);
	if (val) {
		utf8ToUnicode(val, tag->szComment, MAX_PLUGIN_TAG_STR);
	}

	mp4ff_meta_get_genre(file, &val);
	if (val) {
		utf8ToUnicode(val, tag->szGenre, MAX_PLUGIN_TAG_STR);
	}

	mp4ff_meta_get_track(file, &val);
	if (val) {
		tag->nTrackNum = strtol(val, 0, 10);
	}

	mp4ff_meta_get_date(file, &val);
	if (val) {
		strncpy(year, val, 4);
		tag->nYear = strtol(year, 0, 10);
	}
	
	return 1;
}

int aacgettag(MAP_PLUGIN_FILETAG* tag)
{
	if (!mp4state.aacfile)
		return 0;

	if (GetId3TagV2(mp4state.aacfile, tag))
		return 1;
	
	return GetId3TagV1(mp4state.aacfile, tag);
}

int gettag(MAP_PLUGIN_FILETAG* tag)
{
	if (!mp4state.mp4File && !mp4state.aacfile)
		return 0;

	if (mp4state.filetype)
		return aacgettag(tag);
	else
		return mp4gettag(mp4state.mp4file, tag);
}

int mp4getfiletag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* tag)
{
	int ret;
	mp4ff_callback_t mp4cb;
	mp4ff_t* mp4file;

	FILE *fp = _tfopen(pszFile, _T("rb"));
    if (!fp)
        return 0;

	mp4cb.read = read_callback;
    mp4cb.seek = seek_callback;
    mp4cb.user_data = fp;
	mp4file = mp4ff_open_read_metaonly(&mp4cb);
	if (!mp4file) {
		fclose(fp);
		return 0;
	}

	ret = mp4gettag(mp4file, tag);
	mp4ff_close(mp4file);
	fclose(fp);

	return ret;
}

int aacgetfiletag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* tag)
{
	int ret;
	FILE *fp = _tfopen(pszFile, _T("rb"));
	if (!fp)
		return 0;

	ret = GetId3TagV2(fp, tag) ? 1 : 0;
	if (!ret)
		ret = GetId3TagV1(fp, tag) ? 1 : 0;

	fclose(fp);

	return ret;
}

int getfiletag(LPCTSTR pszFile, MAP_PLUGIN_FILETAG* tag)
{
	unsigned char header[8];
	FILE *fp = _tfopen(pszFile, _T("rb"));
    if (!fp)
        return 0;
    
    fread(header, 1, 8, fp);
    fclose(fp);
  
	if (header[4] == 'f' && header[5] == 't' && header[6] == 'y' && header[7] == 'p')
		return mp4getfiletag(pszFile, tag);
	else
		return aacgetfiletag(pszFile, tag);
}

BOOL aac_open_streaming(LPBYTE pbInBuf, DWORD cbInBuf, MAP_PLUGIN_STREMING_INFO* pInfo)
{
	DWORD i, bytesconsumed;
	NeAACDecFrameInfo frameInfo;
	NeAACDecConfigurationPtr config;

	mp4state.hDecoder = NeAACDecOpen();
    if (!mp4state.hDecoder) {
		close();
        return FALSE;
    }

    config = NeAACDecGetCurrentConfiguration(mp4state.hDecoder);
    config->outputFormat = FAAD_FMT_16BIT;
    config->downMatrix = 1;
    NeAACDecSetConfiguration(mp4state.hDecoder, config);

	// search an aac frame header
	for (i = 0; i < cbInBuf; i++) {
		if (is_aac_frame_header(pbInBuf + i, cbInBuf - i)) {
			if (NeAACDecInit(mp4state.hDecoder, pbInBuf + i, cbInBuf - i, (unsigned long*)&mp4state.samplerate, &mp4state.channels) >= 0) {

				bytesconsumed = 0;

				do {
					memset(&frameInfo, 0, sizeof(frameInfo));
					NeAACDecDecode(mp4state.hDecoder, &frameInfo, pbInBuf + i + bytesconsumed, cbInBuf - i - bytesconsumed);
					bytesconsumed += frameInfo.bytesconsumed;
					if (bytesconsumed + i >= cbInBuf)
						break;
				}
				while  (!frameInfo.samples && !frameInfo.error);

				if (frameInfo.samples && !frameInfo.error) {
					mp4state.channels = frameInfo.channels;
					mp4state.samplerate = frameInfo.samplerate;
					mp4state.framesize = (frameInfo.channels != 0) ? frameInfo.samples/frameInfo.channels : 0;
					mp4state.avg_bitrate = (double)mp4state.samplerate / mp4state.framesize * frameInfo.bytesconsumed * 8 / 1000;
					mp4state.initial = 1;
					mp4state.filetype = 1;

					if (/*m_downmix && */(mp4state.channels == 5 || mp4state.channels == 6))
						mp4state.channels = 2;

					pInfo->nAvgBitrate = mp4state.avg_bitrate;
					pInfo->nBitsPerSample = 16;
					pInfo->nChannels = mp4state.channels;
					pInfo->nSampleRate = mp4state.samplerate;

					NeAACDecPostSeekReset(mp4state.hDecoder, -1);
					return TRUE;
				}
			}
		}
	}

	close();
	return FALSE;
}

BOOL open_streaming(LPBYTE pbInBuf, DWORD cbInBuf, MAP_PLUGIN_STREMING_INFO* pInfo)
{
	if (cbInBuf < 8)
		return FALSE;
	
	// mp4 is currently not supported!
	if (pbInBuf[4] == 'f' && pbInBuf[5] == 't' && pbInBuf[6] == 'y' && pbInBuf[7] == 'p')
		return FALSE;
	
	return aac_open_streaming(pbInBuf, cbInBuf, pInfo);
}

int aac_decode_streaming(LPBYTE pbInBuf, DWORD cbInBuf, DWORD* pcbInUsed, WAVEHDR* pHdr)
{
	void *sample_buffer;
	char *buf;
	NeAACDecFrameInfo frameInfo;
	unsigned long samplerate;
	unsigned char channels;
	uint8_t *outbuf = (uint8_t *)pHdr->lpData;
	uint32_t outbuflen = pHdr->dwBufferLength;
	uint32_t *outlen = &pHdr->dwBytesRecorded;

	if (!mp4state.hDecoder)
		return PLUGIN_RET_ERROR;

	*pcbInUsed = 0;

	if (leftoutbuf && leftoutbuflen) {
		if (outbuflen < leftoutbuflen) {
			memcpy(outbuf, leftoutbuf, outbuflen);
			*outlen = outbuflen;

			memmove(leftoutbuf, leftoutbuf + outbuflen, leftoutbuflen - outbuflen);
			leftoutbuflen -= outbuflen;
			return PLUGIN_RET_SUCCESS;
		}
		else {
			memcpy(outbuf, leftoutbuf, leftoutbuflen);
			outbuf += leftoutbuflen;
			*outlen += leftoutbuflen;
			
			free(leftoutbuf);
			leftoutbuflen = 0;
		}
	}

	while (outbuflen - *outlen) {
		if (!is_aac_frame_header(pbInBuf + *pcbInUsed, cbInBuf - *pcbInUsed)) {
			NeAACDecPostSeekReset(mp4state.hDecoder, -1);

			while (*pcbInUsed < outbuflen) {
				(*pcbInUsed)++;	

				if (is_aac_frame_header(pbInBuf + *pcbInUsed, cbInBuf - *pcbInUsed)){
					memset(&frameInfo, 0, sizeof(NeAACDecFrameInfo));
					NeAACDecDecode(mp4state.hDecoder, &frameInfo, mp4state.m_aac_buffer, mp4state.m_aac_bytes_into_buffer);
					if (frameInfo.error == 0) {
						break;
					}					
				}
			}
		}

		memset(&frameInfo, 0, sizeof(NeAACDecFrameInfo));
		sample_buffer = NeAACDecDecode(mp4state.hDecoder, &frameInfo, pbInBuf + *pcbInUsed, cbInBuf - *pcbInUsed);
		*pcbInUsed += frameInfo.bytesconsumed;

		if (frameInfo.error) {
			return PLUGIN_RET_ERROR;
		}

		if (frameInfo.samples > 0 && sample_buffer) {
			buf = (char *)sample_buffer;
			mp4state.initial = 0;
	        
			if (frameInfo.channels == 6 && frameInfo.num_lfe_channels)
				remap_channels((unsigned char*)buf, frameInfo.samples, 16);

			int l = frameInfo.samples * 16 / 8;

			if (outbuflen - *outbuf < l) {
				memcpy(outbuf + *outlen, buf, outbuflen - *outbuf);
				leftoutbuf = (uint8_t*)malloc(l - outbuflen - *outbuf);
				leftoutbuflen = l - outbuflen - *outbuf;
				memcpy(leftoutbuf, buf + outbuflen - *outbuf, leftoutbuflen);

				*outlen = outbuflen;
			}
			else {
				memcpy(outbuf + *outlen, buf, l);
				*outlen += l;
			}
		}

		mp4state.decode_pos_ms += (double)frameInfo.samples * 1000.0 /
			((double)frameInfo.samplerate* (double)frameInfo.channels);

		if (frameInfo.channels > 0 && mp4state.samplerate > 0)
			mp4state.cur_pos_sec += ((double)(frameInfo.samples/frameInfo.channels))/(double)mp4state.samplerate;
	}

	return PLUGIN_RET_SUCCESS;
}

int decode_streaming(LPBYTE pbInBuf, DWORD cbInBuf, DWORD* pcbInUsed, WAVEHDR* pHdr)
{
	// mp4 is currently not supported!
	return aac_decode_streaming(pbInBuf, cbInBuf, pcbInUsed, pHdr);
}

void close_streaming()
{
	close();
}
