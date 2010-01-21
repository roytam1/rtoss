

{ "help|?", "name:s?", do_help_cmd, "[cmd]", "show the help" },

{ "commit", "device:B", do_commit,
"device|all", "commit changes to the disk images (if -snapshot is used) or backing files" },

{ "info", "item:s?", do_info,
"[subcommand]", "show various information about the system state" },

{ "q|quit", "", do_quit,
"", "quit the emulator" },

{ "eject", "force:-f,filename:B", do_eject,
"[-f] device", "eject a removable medium (use -f to force it)" },

{ "change", "device:B,target:F,arg:s?", do_change,
"device filename [format]", "change a removable medium, optional format" },

{ "screendump", "filename:F", do_screen_dump,
"filename", "save screen into PPM image 'filename'" },

{ "logfile", "filename:F", do_logfile,
"filename", "output logs to 'filename'" },

{ "log", "items:s", do_log,
"item1[,...]", "activate logging of the specified items to '/tmp/qemu.log'" },

{ "savevm", "name:s?", do_savevm,
"[tag|id]", "save a VM snapshot. If no tag or id are provided, a new snapshot is created" },

{ "loadvm", "name:s", do_loadvm,
"tag|id", "restore a VM snapshot from its tag or id" },

{ "delvm", "name:s", do_delvm,
"tag|id", "delete a VM snapshot from its tag or id" },

{ "singlestep", "option:s?", do_singlestep,
"[on|off]", "run emulation in singlestep mode or switch to normal mode", },

{ "stop", "", do_stop,
"", "stop emulation", },

{ "c|cont", "", do_cont,
"", "resume emulation", },

{ "gdbserver", "device:s?", do_gdbserver,
"[device]", "start gdbserver on given device (default 'tcp::1234'), stop with 'none'", },

{ "x", "fmt:/,addr:l", do_memory_dump,
"/fmt addr", "virtual memory dump starting at 'addr'", },

{ "xp", "fmt:/,addr:l", do_physical_memory_dump,
"/fmt addr", "physical memory dump starting at 'addr'", },

{ "p|print", "fmt:/,val:l", do_print,
"/fmt expr", "print expression value (use $reg for CPU register access)", },

{ "i", "fmt:/,addr:i,index:i.", do_ioport_read,
"/fmt addr", "I/O port read" },

{ "o", "fmt:/,addr:i,val:i", do_ioport_write,
"/fmt addr value", "I/O port write" },

{ "sendkey", "string:s,hold_time:i?", do_sendkey,
"keys [hold_ms]", "send keys to the VM (e.g. 'sendkey ctrl-alt-f1', default hold time=100 ms)" },

{ "system_reset", "", do_system_reset,
"", "reset the system" },

{ "system_powerdown", "", do_system_powerdown,
"", "send system power down event" },

{ "sum", "start:i,size:i", do_sum,
"addr size", "compute the checksum of a memory region" },

{ "usb_add", "devname:s", do_usb_add,
"device", "add USB device (e.g. 'host:bus.addr' or 'host:vendor_id:product_id')" },

{ "usb_del", "devname:s", do_usb_del,
"device", "remove USB device 'bus.addr'" },

{ "cpu", "index:i", do_cpu_set,
"index", "set the default CPU" },

{ "mouse_move", "dx_str:s,dy_str:s,dz_str:s?", do_mouse_move,
"dx dy [dz]", "send mouse move events" },

{ "mouse_button", "button_state:i", do_mouse_button,
"state", "change mouse button state (1=L, 2=M, 4=R)" },

{ "mouse_set", "index:i", do_mouse_set,
"index", "set which mouse device receives events" },

#ifdef HAS_AUDIO
{ "wavcapture", "path:F,freq:i?,bits:i?,nchannels:i?", do_wav_capture,
"path [frequency [bits [channels]]]",
"capture audio to a wave file (default frequency=44100 bits=16 channels=2)" },
#endif

#ifdef HAS_AUDIO
{ "stopcapture", "n:i", do_stop_capture,
"capture index", "stop capture" },
#endif

{ "memsave", "val:l,size:i,filename:s", do_memory_save,
"addr size file", "save to disk virtual memory dump starting at 'addr' of size 'size'", },

{ "pmemsave", "val:l,size:i,filename:s", do_physical_memory_save,
"addr size file", "save to disk physical memory dump starting at 'addr' of size 'size'", },

{ "boot_set", "bootdevice:s", do_boot_set,
"bootdevice", "define new values for the boot device list" },

#if defined(TARGET_I386)
{ "nmi", "cpu_index:i", do_inject_nmi,
"cpu", "inject an NMI on the given CPU", },
#endif

{ "migrate", "detach:-d,uri:s", do_migrate,
"[-d] uri", "migrate to URI (using -d to not wait for completion)" },

{ "migrate_cancel", "", do_migrate_cancel,
"", "cancel the current VM migration" },

{ "migrate_set_speed", "value:s", do_migrate_set_speed,
"value", "set maximum speed (in bytes) for migrations" },

{ "migrate_set_downtime", "value:s", do_migrate_set_downtime,
"value", "set maximum tolerated downtime (in seconds) for migrations" },


#if defined(TARGET_I386)
{ "drive_add", "pci_addr:s,opts:s", drive_hot_add,
"[[<domain>:]<bus>:]<slot>\n"
"[file=file][,if=type][,bus=n]\n"
"[,unit=m][,media=d][index=i]\n"
"[,cyls=c,heads=h,secs=s[,trans=t]]\n"
"[snapshot=on|off][,cache=on|off]",
"add drive to PCI storage controller" },
#endif

#if defined(TARGET_I386)
{ "pci_add", "pci_addr:s,type:s,opts:s?", pci_device_hot_add, "auto|[[<domain>:]<bus>:]<slot> nic|storage [[vlan=n][,macaddr=addr][,model=type]] [file=file][,if=type][,bus=nr]...", "hot-add PCI device" },
#endif

#if defined(TARGET_I386)
{ "pci_del", "pci_addr:s", do_pci_device_hot_remove, "[[<domain>:]<bus>:]<slot>", "hot remove PCI device" },
#endif

{ "host_net_add", "device:s,opts:s?", net_host_device_add,
"tap|user|socket|vde|dump [options]", "add host VLAN client" },

{ "host_net_remove", "vlan_id:i,device:s", net_host_device_remove,
"vlan_id name", "remove host VLAN client" },

#ifdef CONFIG_SLIRP
{ "hostfwd_add", "arg1:s,arg2:s?,arg3:s?", net_slirp_hostfwd_add,
"[vlan_id name] [tcp|udp]:[hostaddr]:hostport-[guestaddr]:guestport",
"redirect TCP or UDP connections from host to guest (requires -net user)" },
{ "hostfwd_remove", "arg1:s,arg2:s?,arg3:s?", net_slirp_hostfwd_remove,
"[vlan_id name] [tcp|udp]:[hostaddr]:hostport",
"remove host-to-guest TCP or UDP redirection" },
#endif

{ "balloon", "value:i", do_balloon,
"target", "request VM to change it's memory allocation (in MB)" },

{ "set_link", "name:s,up_or_down:s", do_set_link,
"name up|down", "change the link status of a network adapter" },

{ "watchdog_action", "action:s", do_watchdog_action,
"[reset|shutdown|poweroff|pause|debug|none]", "change watchdog action" },

{ "acl_show", "aclname:s", do_acl_show, "aclname",
"list rules in the access control list" },

{ "acl_policy", "aclname:s,policy:s", do_acl_policy, "aclname allow|deny",
"set default access control list policy" },

{ "acl_add", "aclname:s,match:s,policy:s,index:i?", do_acl_add, "aclname match allow|deny [index]",
"add a match rule to the access control list" },

{ "acl_remove", "aclname:s,match:s", do_acl_remove, "aclname match",
"remove a match rule from the access control list" },

{ "acl_reset", "aclname:s", do_acl_reset, "aclname",
"reset the access control list" },

#if defined(TARGET_I386)
{ "mce", "cpu_index:i,bank:i,status:l,mcg_status:l,addr:l,misc:l", do_inject_mce, "cpu bank status mcgstatus addr misc", "inject a MCE on the given CPU"},
#endif

{ "getfd", "fdname:s", do_getfd, "getfd name",
"receive a file descriptor via SCM rights and assign it a name" },

{ "closefd", "fdname:s", do_closefd, "closefd name",
"close a file descriptor previously passed via SCM rights" },

