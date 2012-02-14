extern unsigned long pci_mem_start, pci_mem_end;
struct bios_info {
    unsigned char com1_present:1;
    unsigned char com2_present:1;
    unsigned char lpt1_present:1;
    unsigned char hpet_present:1;
    unsigned long pci_min, pci_len;
    unsigned long madt_csum_addr;
    unsigned long madt_lapic0_addr;
    unsigned long bios32_entry;
};
 #asm
.rom
.org 0x0000
use16 386
MACRO HALT
  ;; the HALT macro is called with the line number of the HALT call.
  ;; The line number is then sent to the 0x400, causing Bochs/Plex
  ;; to print a BX_PANIC message. This will normally halt the simulation
  ;; with a message such as "BIOS panic at rombios.c, line 4091".
  ;; However, users can choose to make panics non-fatal and continue.
  mov dx,#0x400
  mov ax,#?1
  out dx,ax
MEND
MACRO JMP_AP
  db 0xea
  dw ?2
  dw ?1
MEND
MACRO SET_INT_VECTOR
  mov ax, ?3
  mov ?1*4, ax
  mov ax, ?2
  mov ?1*4+2, ax
MEND
 #endasm
typedef unsigned char Bit8u;
typedef unsigned short Bit16u;
typedef unsigned short bx_bool;
typedef unsigned long Bit32u;
  void memsetb(seg,offset,value,count);
  void memcpyb(dseg,doffset,sseg,soffset,count);
  void memcpyd(dseg,doffset,sseg,soffset,count);
    void
  memsetb(seg,offset,value,count)
    Bit16u seg;
    Bit16u offset;
    Bit16u value;
    Bit16u count;
  {
  #asm
    push bp
    mov bp, sp
      push ax
      push cx
      push es
      push di
      mov cx, 10[bp] ; count
      test cx, cx
      je memsetb_end
      mov ax, 4[bp] ; segment
      mov es, ax
      mov ax, 6[bp] ; offset
      mov di, ax
      mov al, 8[bp] ; value
      cld
      rep
       stosb
  memsetb_end:
      pop di
      pop es
      pop cx
      pop ax
    pop bp
  #endasm
  }
    void
  memcpyb(dseg,doffset,sseg,soffset,count)
    Bit16u dseg;
    Bit16u doffset;
    Bit16u sseg;
    Bit16u soffset;
    Bit16u count;
  {
  #asm
    push bp
    mov bp, sp
      push ax
      push cx
      push es
      push di
      push ds
      push si
      mov cx, 12[bp] ; count
      test cx, cx
      je memcpyb_end
      mov ax, 4[bp] ; dsegment
      mov es, ax
      mov ax, 6[bp] ; doffset
      mov di, ax
      mov ax, 8[bp] ; ssegment
      mov ds, ax
      mov ax, 10[bp] ; soffset
      mov si, ax
      cld
      rep
       movsb
  memcpyb_end:
      pop si
      pop ds
      pop di
      pop es
      pop cx
      pop ax
    pop bp
  #endasm
  }
    void
  memcpyd(dseg,doffset,sseg,soffset,count)
    Bit16u dseg;
    Bit16u doffset;
    Bit16u sseg;
    Bit16u soffset;
    Bit16u count;
  {
  #asm
    push bp
    mov bp, sp
      push ax
      push cx
      push es
      push di
      push ds
      push si
      mov cx, 12[bp] ; count
      test cx, cx
      je memcpyd_end
      mov ax, 4[bp] ; dsegment
      mov es, ax
      mov ax, 6[bp] ; doffset
      mov di, ax
      mov ax, 8[bp] ; ssegment
      mov ds, ax
      mov ax, 10[bp] ; soffset
      mov si, ax
      cld
      rep
       movsd
  memcpyd_end:
      pop si
      pop ds
      pop di
      pop es
      pop cx
      pop ax
    pop bp
  #endasm
  }
  static Bit32u read_dword();
  static void write_dword();
    Bit32u
  read_dword(seg, offset)
    Bit16u seg;
    Bit16u offset;
  {
  #asm
    push bp
    mov bp, sp
      push bx
      push ds
      mov ax, 4[bp] ; segment
      mov ds, ax
      mov bx, 6[bp] ; offset
      mov ax, [bx]
      add bx, #2
      mov dx, [bx]
      ;; ax = return value (word)
      ;; dx = return value (word)
      pop ds
      pop bx
    pop bp
  #endasm
  }
    void
  write_dword(seg, offset, data)
    Bit16u seg;
    Bit16u offset;
    Bit32u data;
  {
  #asm
    push bp
    mov bp, sp
      push ax
      push bx
      push ds
      mov ax, 4[bp] ; segment
      mov ds, ax
      mov bx, 6[bp] ; offset
      mov ax, 8[bp] ; data word
      mov [bx], ax ; write data word
      add bx, #2
      mov ax, 10[bp] ; data word
      mov [bx], ax ; write data word
      pop ds
      pop bx
      pop ax
    pop bp
  #endasm
  }
  #asm
  ;; and function
  landl:
  landul:
    SEG SS
      and ax,[di]
    SEG SS
      and bx,2[di]
    ret
  ;; add function
  laddl:
  laddul:
    SEG SS
      add ax,[di]
    SEG SS
      adc bx,2[di]
    ret
  ;; cmp function
  lcmpl:
  lcmpul:
    and eax, #0x0000FFFF
    shl ebx, #16
    or eax, ebx
    shr ebx, #16
    SEG SS
      cmp eax, dword ptr [di]
    ret
  ;; sub function
  lsubl:
  lsubul:
    SEG SS
    sub ax,[di]
    SEG SS
    sbb bx,2[di]
    ret
  ;; mul function
  lmull:
  lmulul:
    and eax, #0x0000FFFF
    shl ebx, #16
    or eax, ebx
    SEG SS
    mul eax, dword ptr [di]
    mov ebx, eax
    shr ebx, #16
    ret
  ;; dec function
  ldecl:
  ldecul:
    SEG SS
    dec dword ptr [bx]
    ret
  ;; or function
  lorl:
  lorul:
    SEG SS
    or ax,[di]
    SEG SS
    or bx,2[di]
    ret
  ;; inc function
  lincl:
  lincul:
    SEG SS
    inc dword ptr [bx]
    ret
  ;; tst function
  ltstl:
  ltstul:
    and eax, #0x0000FFFF
    shl ebx, #16
    or eax, ebx
    shr ebx, #16
    test eax, eax
    ret
  ;; sr function
  lsrul:
    mov cx,di
    jcxz lsr_exit
    and eax, #0x0000FFFF
    shl ebx, #16
    or eax, ebx
  lsr_loop:
    shr eax, #1
    loop lsr_loop
    mov ebx, eax
    shr ebx, #16
  lsr_exit:
    ret
  ;; sl function
  lsll:
  lslul:
    mov cx,di
    jcxz lsl_exit
    and eax, #0x0000FFFF
    shl ebx, #16
    or eax, ebx
  lsl_loop:
    shl eax, #1
    loop lsl_loop
    mov ebx, eax
    shr ebx, #16
  lsl_exit:
    ret
  idiv_:
    cwd
    idiv bx
    ret
  idiv_u:
    xor dx,dx
    div bx
    ret
  ldivul:
    and eax, #0x0000FFFF
    shl ebx, #16
    or eax, ebx
    xor edx, edx
    SEG SS
    mov bx, 2[di]
    shl ebx, #16
    SEG SS
    mov bx, [di]
    div ebx
    mov ebx, eax
    shr ebx, #16
    ret
  #endasm
typedef struct {
  unsigned char filler1[0x400];
  unsigned char filler2[0x6c];
  Bit16u ticks_low;
  Bit16u ticks_high;
  Bit8u midnight_flag;
  } bios_data_t;
  typedef struct {
    Bit16u heads;
    Bit16u cylinders;
    Bit16u spt;
    } chs_t;
  typedef struct {
    Bit16u iobase1;
    Bit16u iobase2;
    Bit8u prefix;
    Bit8u unused;
    Bit8u irq;
    Bit8u blkcount;
    Bit8u dma;
    Bit8u pio;
    Bit16u options;
    Bit16u reserved;
    Bit8u revision;
    Bit8u checksum;
    } dpte_t;
  typedef struct {
    Bit8u iface;
    Bit16u iobase1;
    Bit16u iobase2;
    Bit8u irq;
    } ata_channel_t;
  typedef struct {
    Bit8u type;
    Bit8u device;
    Bit8u removable;
    Bit8u lock;
    Bit8u mode;
    Bit16u blksize;
    Bit8u translation;
    chs_t lchs;
    chs_t pchs;
    Bit32u sectors_low;
    Bit32u sectors_high;
    } ata_device_t;
  typedef struct {
    ata_channel_t channels[4];
    ata_device_t devices[(4*2)];
    Bit8u hdcount, hdidmap[(4*2)];
    Bit8u cdcount, cdidmap[(4*2)];
    dpte_t dpte;
    Bit16u trsfsectors;
    Bit32u trsfbytes;
    } ata_t;
  typedef struct {
    Bit8u active;
    Bit8u media;
    Bit8u emulated_drive;
    Bit8u controller_index;
    Bit16u device_spec;
    Bit32u ilba;
    Bit16u buffer_segment;
    Bit16u load_segment;
    Bit16u sector_count;
    chs_t vdevice;
    } cdemu_t;
Bit32u TCGInterruptHandler ();
void tcpa_acpi_init ();
Bit32u tcpa_extend_acpi_log ();
void tcpa_calling_int19h ();
void tcpa_returned_int19h ();
void tcpa_add_event_separators ();
void tcpa_wake_event ();
void tcpa_add_bootdevice ();
void tcpa_start_option_rom_scan ();
void tcpa_option_rom ();
void tcpa_ipl ();
void tcpa_measure_post ();
Bit32u tcpa_initialize_tpm ();
Bit32u get_s3_waking_vector ();
Bit32u pmm ();
  typedef struct {
    unsigned char ebda_size;
    unsigned char cmos_shutdown_status;
    unsigned char filler1[0x3B];
    unsigned char fdpt0[0x10];
    unsigned char fdpt1[0x10];
    unsigned char filler2[0xC4];
    ata_t ata;
    cdemu_t cdemu;
    } ebda_data_t;
  typedef struct {
    Bit8u size;
    Bit8u reserved;
    Bit16u count;
    Bit16u offset;
    Bit16u segment;
    Bit32u lba1;
    Bit32u lba2;
    } int13ext_t;
  typedef struct {
    Bit16u size;
    Bit16u infos;
    Bit32u cylinders;
    Bit32u heads;
    Bit32u spt;
    Bit32u sector_count1;
    Bit32u sector_count2;
    Bit16u blksize;
    Bit16u dpte_offset;
    Bit16u dpte_segment;
    Bit16u key;
    Bit8u dpi_length;
    Bit8u reserved1;
    Bit16u reserved2;
    Bit8u host_bus[4];
    Bit8u iface_type[8];
    Bit8u iface_path[8];
    Bit8u device_path[8];
    Bit8u reserved3;
    Bit8u checksum;
    } dpt_t;
typedef struct {
  union {
    struct {
      Bit16u di, si, bp, sp;
      Bit16u bx, dx, cx, ax;
      } r16;
    struct {
      Bit16u filler[4];
      Bit8u bl, bh, dl, dh, cl, ch, al, ah;
      } r8;
    } u;
  } pusha_regs_t;
typedef struct {
 union {
  struct {
    Bit32u edi, esi, ebp, esp;
    Bit32u ebx, edx, ecx, eax;
    } r32;
  struct {
    Bit16u di, filler1, si, filler2, bp, filler3, sp, filler4;
    Bit16u bx, filler5, dx, filler6, cx, filler7, ax, filler8;
    } r16;
  struct {
    Bit32u filler[4];
    Bit8u bl, bh;
    Bit16u filler1;
    Bit8u dl, dh;
    Bit16u filler2;
    Bit8u cl, ch;
    Bit16u filler3;
    Bit8u al, ah;
    Bit16u filler4;
    } r8;
  } u;
} pushad_regs_t;
typedef struct {
  union {
    struct {
      Bit16u flags;
      } r16;
    struct {
      Bit8u flagsl;
      Bit8u flagsh;
      } r8;
    } u;
  } flags_t;
typedef struct {
  Bit16u ip;
  Bit16u cs;
  flags_t flags;
  } iret_addr_t;
typedef struct {
  Bit16u type;
  Bit16u flags;
  Bit32u vector;
  Bit32u description;
  Bit32u reserved;
  } ipl_entry_t;
static Bit8u inb();
static Bit8u inb_cmos();
static void outb();
static void outb_cmos();
static Bit16u inw();
static void outw();
static void init_rtc();
static bx_bool rtc_updating();
static Bit8u read_byte();
static Bit16u read_word();
static void write_byte();
static void write_word();
static void bios_printf();
static Bit8u inhibit_mouse_int_and_events();
static void enable_mouse_int_and_events();
static Bit8u send_to_mouse_ctrl();
static Bit8u get_mouse_data();
static void set_kbd_command_byte();
static void int09_function();
static void int13_harddisk();
static void int13_cdrom();
static void int13_cdemu();
static void int13_eltorito();
static void int13_diskette_function();
static void int14_function();
static void int15_function();
static void int16_function();
static void int17_function();
static void int18_function();
static void int1a_function();
static void int70_function();
static void int74_function();
static Bit16u get_CS();
static Bit16u get_SS();
static unsigned int enqueue_key();
static unsigned int dequeue_key();
static void get_hd_geometry();
static void set_diskette_ret_status();
static void set_diskette_current_cyl();
static void determine_floppy_media();
static bx_bool floppy_drive_exists();
static bx_bool floppy_drive_recal();
static bx_bool floppy_media_known();
static bx_bool floppy_media_sense();
static bx_bool set_enable_a20();
static void debugger_on();
static void debugger_off();
static void keyboard_init();
static void keyboard_panic();
static void shutdown_status_panic();
static void nmi_handler_msg();
static void delay_ticks();
static void delay_ticks_and_check_for_keystroke();
static void interactive_bootkey();
static void print_bios_banner();
static void print_boot_device();
static void print_boot_failure();
static void print_cdromboot_failure();
void ata_init();
void ata_detect();
void ata_reset();
Bit16u ata_cmd_non_data();
Bit16u ata_cmd_data_in();
Bit16u ata_cmd_data_out();
Bit16u ata_cmd_packet();
Bit16u atapi_get_sense();
Bit16u atapi_is_ready();
Bit16u atapi_is_cdrom();
void cdemu_init();
Bit8u cdemu_isactive();
Bit8u cdemu_emulated_drive();
Bit16u cdrom_boot();
static char bios_cvs_version_string[] = "$Revision: 1.221 $ $Date: 2008/12/07 17:32:29 $";
static struct {
  Bit16u normal;
  Bit16u shift;
  Bit16u control;
  Bit16u alt;
  Bit8u lock_flags;
  } scan_to_scanascii[0x58 + 1] = {
      { 0, 0, 0, 0, 0 },
      { 0x011b, 0x011b, 0x011b, 0x0100, 0 },
      { 0x0231, 0x0221, 0, 0x7800, 0 },
      { 0x0332, 0x0340, 0x0300, 0x7900, 0 },
      { 0x0433, 0x0423, 0, 0x7a00, 0 },
      { 0x0534, 0x0524, 0, 0x7b00, 0 },
      { 0x0635, 0x0625, 0, 0x7c00, 0 },
      { 0x0736, 0x075e, 0x071e, 0x7d00, 0 },
      { 0x0837, 0x0826, 0, 0x7e00, 0 },
      { 0x0938, 0x092a, 0, 0x7f00, 0 },
      { 0x0a39, 0x0a28, 0, 0x8000, 0 },
      { 0x0b30, 0x0b29, 0, 0x8100, 0 },
      { 0x0c2d, 0x0c5f, 0x0c1f, 0x8200, 0 },
      { 0x0d3d, 0x0d2b, 0, 0x8300, 0 },
      { 0x0e08, 0x0e08, 0x0e7f, 0, 0 },
      { 0x0f09, 0x0f00, 0, 0, 0 },
      { 0x1071, 0x1051, 0x1011, 0x1000, 0x40 },
      { 0x1177, 0x1157, 0x1117, 0x1100, 0x40 },
      { 0x1265, 0x1245, 0x1205, 0x1200, 0x40 },
      { 0x1372, 0x1352, 0x1312, 0x1300, 0x40 },
      { 0x1474, 0x1454, 0x1414, 0x1400, 0x40 },
      { 0x1579, 0x1559, 0x1519, 0x1500, 0x40 },
      { 0x1675, 0x1655, 0x1615, 0x1600, 0x40 },
      { 0x1769, 0x1749, 0x1709, 0x1700, 0x40 },
      { 0x186f, 0x184f, 0x180f, 0x1800, 0x40 },
      { 0x1970, 0x1950, 0x1910, 0x1900, 0x40 },
      { 0x1a5b, 0x1a7b, 0x1a1b, 0, 0 },
      { 0x1b5d, 0x1b7d, 0x1b1d, 0, 0 },
      { 0x1c0d, 0x1c0d, 0x1c0a, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0x1e61, 0x1e41, 0x1e01, 0x1e00, 0x40 },
      { 0x1f73, 0x1f53, 0x1f13, 0x1f00, 0x40 },
      { 0x2064, 0x2044, 0x2004, 0x2000, 0x40 },
      { 0x2166, 0x2146, 0x2106, 0x2100, 0x40 },
      { 0x2267, 0x2247, 0x2207, 0x2200, 0x40 },
      { 0x2368, 0x2348, 0x2308, 0x2300, 0x40 },
      { 0x246a, 0x244a, 0x240a, 0x2400, 0x40 },
      { 0x256b, 0x254b, 0x250b, 0x2500, 0x40 },
      { 0x266c, 0x264c, 0x260c, 0x2600, 0x40 },
      { 0x273b, 0x273a, 0, 0, 0 },
      { 0x2827, 0x2822, 0, 0, 0 },
      { 0x2960, 0x297e, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0x2b5c, 0x2b7c, 0x2b1c, 0, 0 },
      { 0x2c7a, 0x2c5a, 0x2c1a, 0x2c00, 0x40 },
      { 0x2d78, 0x2d58, 0x2d18, 0x2d00, 0x40 },
      { 0x2e63, 0x2e43, 0x2e03, 0x2e00, 0x40 },
      { 0x2f76, 0x2f56, 0x2f16, 0x2f00, 0x40 },
      { 0x3062, 0x3042, 0x3002, 0x3000, 0x40 },
      { 0x316e, 0x314e, 0x310e, 0x3100, 0x40 },
      { 0x326d, 0x324d, 0x320d, 0x3200, 0x40 },
      { 0x332c, 0x333c, 0, 0, 0 },
      { 0x342e, 0x343e, 0, 0, 0 },
      { 0x352f, 0x353f, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0x372a, 0x372a, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0x3920, 0x3920, 0x3920, 0x3920, 0 },
      { 0, 0, 0, 0, 0 },
      { 0x3b00, 0x5400, 0x5e00, 0x6800, 0 },
      { 0x3c00, 0x5500, 0x5f00, 0x6900, 0 },
      { 0x3d00, 0x5600, 0x6000, 0x6a00, 0 },
      { 0x3e00, 0x5700, 0x6100, 0x6b00, 0 },
      { 0x3f00, 0x5800, 0x6200, 0x6c00, 0 },
      { 0x4000, 0x5900, 0x6300, 0x6d00, 0 },
      { 0x4100, 0x5a00, 0x6400, 0x6e00, 0 },
      { 0x4200, 0x5b00, 0x6500, 0x6f00, 0 },
      { 0x4300, 0x5c00, 0x6600, 0x7000, 0 },
      { 0x4400, 0x5d00, 0x6700, 0x7100, 0 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0x4700, 0x4737, 0x7700, 0, 0x20 },
      { 0x4800, 0x4838, 0, 0, 0x20 },
      { 0x4900, 0x4939, 0x8400, 0, 0x20 },
      { 0x4a2d, 0x4a2d, 0, 0, 0 },
      { 0x4b00, 0x4b34, 0x7300, 0, 0x20 },
      { 0x4c00, 0x4c35, 0, 0, 0x20 },
      { 0x4d00, 0x4d36, 0x7400, 0, 0x20 },
      { 0x4e2b, 0x4e2b, 0, 0, 0 },
      { 0x4f00, 0x4f31, 0x7500, 0, 0x20 },
      { 0x5000, 0x5032, 0, 0, 0x20 },
      { 0x5100, 0x5133, 0x7600, 0, 0x20 },
      { 0x5200, 0x5230, 0, 0, 0x20 },
      { 0x5300, 0x532e, 0, 0, 0x20 },
      { 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0 },
      { 0x565c, 0x567c, 0, 0, 0 },
      { 0x8500, 0x8700, 0x8900, 0x8b00, 0 },
      { 0x8600, 0x8800, 0x8a00, 0x8c00, 0 },
      };
  Bit8u
inb(port)
  Bit16u port;
{
 #asm
  push bp
  mov bp, sp
    push dx
    mov dx, 4[bp]
    in al, dx
    pop dx
  pop bp
 #endasm
}
  Bit16u
inw(port)
  Bit16u port;
{
 #asm
  push bp
  mov bp, sp
    push dx
    mov dx, 4[bp]
    in ax, dx
    pop dx
  pop bp
 #endasm
}
  void
outb(port, val)
  Bit16u port;
  Bit8u val;
{
 #asm
  push bp
  mov bp, sp
    push ax
    push dx
    mov dx, 4[bp]
    mov al, 6[bp]
    out dx, al
    pop dx
    pop ax
  pop bp
 #endasm
}
  void
outw(port, val)
  Bit16u port;
  Bit16u val;
{
 #asm
  push bp
  mov bp, sp
    push ax
    push dx
    mov dx, 4[bp]
    mov ax, 6[bp]
    out dx, ax
    pop dx
    pop ax
  pop bp
 #endasm
}
  void
outb_cmos(cmos_reg, val)
  Bit8u cmos_reg;
  Bit8u val;
{
 #asm
  push bp
  mov bp, sp
    mov al, 4[bp] ;; cmos_reg
    out 0x70, al
    mov al, 6[bp] ;; val
    out 0x71, al
  pop bp
 #endasm
}
  Bit8u
inb_cmos(cmos_reg)
  Bit8u cmos_reg;
{
 #asm
  push bp
  mov bp, sp
    mov al, 4[bp] ;; cmos_reg
    out 0x70, al
    in al, 0x71
  pop bp
 #endasm
}
  void
init_rtc()
{
  outb_cmos(0x0a, 0x26);
  outb_cmos(0x0b, 0x02);
  inb_cmos(0x0c);
  inb_cmos(0x0d);
}
  bx_bool
rtc_updating()
{
  Bit16u count;
  count = 25000;
  while (--count != 0) {
    if ( (inb_cmos(0x0a) & 0x80) == 0 )
      return(0);
    }
  return(1);
}
  Bit8u
read_byte(seg, offset)
  Bit16u seg;
  Bit16u offset;
{
 #asm
  push bp
  mov bp, sp
    push bx
    push ds
    mov ax, 4[bp] ; segment
    mov ds, ax
    mov bx, 6[bp] ; offset
    mov al, [bx]
    ;; al = return value (byte)
    pop ds
    pop bx
  pop bp
 #endasm
}
  Bit16u
read_word(seg, offset)
  Bit16u seg;
  Bit16u offset;
{
 #asm
  push bp
  mov bp, sp
    push bx
    push ds
    mov ax, 4[bp] ; segment
    mov ds, ax
    mov bx, 6[bp] ; offset
    mov ax, [bx]
    ;; ax = return value (word)
    pop ds
    pop bx
  pop bp
 #endasm
}
  void
write_byte(seg, offset, data)
  Bit16u seg;
  Bit16u offset;
  Bit8u data;
{
 #asm
  push bp
  mov bp, sp
    push ax
    push bx
    push ds
    mov ax, 4[bp] ; segment
    mov ds, ax
    mov bx, 6[bp] ; offset
    mov al, 8[bp] ; data byte
    mov [bx], al ; write data byte
    pop ds
    pop bx
    pop ax
  pop bp
 #endasm
}
  void
write_word(seg, offset, data)
  Bit16u seg;
  Bit16u offset;
  Bit16u data;
{
 #asm
  push bp
  mov bp, sp
    push ax
    push bx
    push ds
    mov ax, 4[bp] ; segment
    mov ds, ax
    mov bx, 6[bp] ; offset
    mov ax, 8[bp] ; data word
    mov [bx], ax ; write data word
    pop ds
    pop bx
    pop ax
  pop bp
 #endasm
}
  Bit16u
get_CS()
{
 #asm
  mov ax, cs
 #endasm
}
  Bit16u
get_SS()
{
 #asm
  mov ax, ss
 #endasm
}
void
fixup_base_mem_in_k()
{
  Bit32u base_mem = read_dword((Bit16u)(0x000EA100 >> 4), 0x8 + 8);
  write_word(0x40, 0x13, base_mem >> 10);
}
void enable_rom_write_access()
{
    outb(0x10, 0);
}
void disable_rom_write_access()
{
    outb(0x10, 1);
}
  void
wrch(c)
  Bit8u c;
{
  #asm
  push bp
  mov bp, sp
  push bx
  mov ah, #0x0e
  mov al, 4[bp]
  xor bx,bx
  int #0x10
  pop bx
  pop bp
  #endasm
}
  void
send(action, c)
  Bit16u action;
  Bit8u c;
{
  outb(0xE9, c);
  if (action & 8) outb(0x403, c);
  if (action & 4) outb(0x402, c);
  if (action & 2) {
    if (c == '\n') wrch('\r');
    wrch(c);
  }
}
  void
put_int(action, val, width, neg)
  Bit16u action;
  short val, width;
  bx_bool neg;
{
  short nval = val / 10;
  if (nval)
    put_int(action, nval, width - 1, neg);
  else {
    while (--width > 0) send(action, ' ');
    if (neg) send(action, '-');
  }
  send(action, val - (nval * 10) + '0');
}
  void
put_uint(action, val, width, neg)
  Bit16u action;
  unsigned short val;
  short width;
  bx_bool neg;
{
  unsigned short nval = val / 10;
  if (nval)
    put_uint(action, nval, width - 1, neg);
  else {
    while (--width > 0) send(action, ' ');
    if (neg) send(action, '-');
  }
  send(action, val - (nval * 10) + '0');
}
  void
put_luint(action, val, width, neg)
  Bit16u action;
  unsigned long val;
  short width;
  bx_bool neg;
{
  unsigned long nval = val / 10;
  if (nval)
    put_luint(action, nval, width - 1, neg);
  else {
    while (--width > 0) send(action, ' ');
    if (neg) send(action, '-');
  }
  send(action, val - (nval * 10) + '0');
}
void put_str(action, segment, offset)
  Bit16u action;
  Bit16u segment;
  Bit16u offset;
{
  Bit8u c;
  while (c = read_byte(segment, offset)) {
    send(action, c);
    offset++;
  }
}
  void
delay_ticks(ticks)
  Bit16u ticks;
{
  long ticks_to_wait, delta;
  Bit32u prev_ticks, t;
 #asm
  pushf
  sti
 #endasm
  ticks_to_wait = ticks;
  prev_ticks = read_dword(0x0, 0x46c);
  do
  {
 #asm
    hlt
 #endasm
    t = read_dword(0x0, 0x46c);
    if (t > prev_ticks)
    {
      delta = t - prev_ticks;
      ticks_to_wait -= delta;
    }
    else if (t < prev_ticks)
    {
      ticks_to_wait -= t;
    }
    prev_ticks = t;
  } while (ticks_to_wait > 0);
 #asm
  cli
  popf
 #endasm
}
  Bit8u
check_for_keystroke()
{
 #asm
  mov ax, #0x100
  int #0x16
  jz no_key
  mov al, #1
  jmp done
no_key:
  xor al, al
done:
 #endasm
}
  Bit8u
get_keystroke()
{
 #asm
  mov ax, #0x0
  int #0x16
  xchg ah, al
 #endasm
}
  void
delay_ticks_and_check_for_keystroke(ticks, count)
  Bit16u ticks, count;
{
  Bit16u i;
  for (i = 1; i <= count; i++) {
    delay_ticks(ticks);
    if (check_for_keystroke())
      break;
  }
}
  void
bios_printf(action, s)
  Bit16u action;
  Bit8u *s;
{
  Bit8u c, format_char;
  bx_bool in_format;
  short i;
  Bit16u *arg_ptr;
  Bit16u arg_seg, arg, nibble, hibyte, shift_count, format_width, hexadd;
  arg_ptr = &s;
  arg_seg = get_SS();
  in_format = 0;
  format_width = 0;
  if ((action & (2 | 4 | 1)) == (2 | 4 | 1)) {
    outb(0x401, 0x00);
    bios_printf (2, "FATAL: ");
  }
  while (c = read_byte(get_CS(), s)) {
    if ( c == '%' ) {
      in_format = 1;
      format_width = 0;
      }
    else if (in_format) {
      if ( (c>='0') && (c<='9') ) {
        format_width = (format_width * 10) + (c - '0');
        }
      else {
        arg_ptr++;
        arg = read_word(arg_seg, arg_ptr);
        if (c == 'x' || c == 'X') {
          if (format_width == 0)
            format_width = 4;
          if (c == 'x')
            hexadd = 'a';
          else
            hexadd = 'A';
          for (i=format_width-1; i>=0; i--) {
            nibble = (arg >> (4 * i)) & 0x000f;
            send (action, (nibble<=9)? (nibble+'0') : (nibble-10+hexadd));
            }
          }
        else if (c == 'u') {
          put_uint(action, arg, format_width, 0);
          }
        else if (c == 'l') {
          s++;
          c = read_byte(get_CS(), s);
          arg_ptr++;
          hibyte = read_word(arg_seg, arg_ptr);
          if (c == 'd') {
            if (hibyte & 0x8000)
              put_luint(action, 0L-(((Bit32u) hibyte << 16) | arg), format_width-1, 1);
            else
              put_luint(action, ((Bit32u) hibyte << 16) | arg, format_width, 0);
           }
          else if (c == 'u') {
            put_luint(action, ((Bit32u) hibyte << 16) | arg, format_width, 0);
           }
          else if (c == 'x' || c == 'X')
           {
            if (format_width == 0)
              format_width = 8;
            if (c == 'x')
              hexadd = 'a';
            else
              hexadd = 'A';
            for (i=format_width-1; i>=0; i--) {
              nibble = ((((Bit32u) hibyte <<16) | arg) >> (4 * i)) & 0x000f;
              send (action, (nibble<=9)? (nibble+'0') : (nibble-10+hexadd));
              }
           }
          }
        else if (c == 'd') {
          if (arg & 0x8000)
            put_int(action, -arg, format_width - 1, 1);
          else
            put_int(action, arg, format_width, 0);
          }
        else if (c == 's') {
          put_str(action, get_CS(), arg);
          }
        else if (c == 'S') {
          hibyte = arg;
          arg_ptr++;
          arg = read_word(arg_seg, arg_ptr);
          put_str(action, hibyte, arg);
          }
        else if (c == 'c') {
          send(action, arg);
          }
        else
          bios_printf((2 | 4 | 1), "bios_printf: unknown format\n");
          in_format = 0;
        }
      }
    else {
      send(action, c);
      }
    s ++;
    }
  if (action & 1) {
 #asm
    cli
 halt2_loop:
    hlt
    jmp halt2_loop
 #endasm
    }
}
  void
keyboard_init()
{
    Bit16u max;
    max=0xffff;
    while ( (inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x00);
    max=0x2000;
    while (--max > 0) {
        outb(0x80, 0x00);
        if (inb(0x64) & 0x01) {
            inb(0x60);
            max = 0x2000;
            }
        }
    outb(0x64, 0xaa);
    max=0xffff;
    while ( (inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x00);
    if (max==0x0) keyboard_panic(00);
    max=0xffff;
    while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x01);
    if (max==0x0) keyboard_panic(01);
    if ((inb(0x60) != 0x55)){
        keyboard_panic(991);
    }
    outb(0x64,0xab);
    max=0xffff;
    while ((inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x10);
    if (max==0x0) keyboard_panic(10);
    max=0xffff;
    while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x11);
    if (max==0x0) keyboard_panic(11);
    if ((inb(0x60) != 0x00)) {
        keyboard_panic(992);
    }
    outb(0x64,0xae);
    outb(0x64,0xa8);
    outb(0x60, 0xff);
    max=0xffff;
    while ((inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x20);
    if (max==0x0) keyboard_panic(20);
    max=0xffff;
    while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x21);
    if (max==0x0) keyboard_panic(21);
    if ((inb(0x60) != 0xfa)) {
        keyboard_panic(993);
    }
    max=0xffff;
    while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x31);
    if (max==0x0) keyboard_panic(31);
    if ((inb(0x60) != 0xaa)) {
        keyboard_panic(994);
    }
    outb(0x60, 0xf5);
    max=0xffff;
    while ((inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x40);
    if (max==0x0) keyboard_panic(40);
    max=0xffff;
    while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x41);
    if (max==0x0) keyboard_panic(41);
    if ((inb(0x60) != 0xfa)) {
        keyboard_panic(995);
    }
    outb(0x64, 0x60);
    max=0xffff;
    while ((inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x50);
    if (max==0x0) keyboard_panic(50);
    outb(0x60, 0x61);
    max=0xffff;
    while ((inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x60);
    if (max==0x0) keyboard_panic(60);
    outb(0x60, 0xf4);
    max=0xffff;
    while ((inb(0x64) & 0x02) && (--max>0)) outb(0x80, 0x70);
    if (max==0x0) keyboard_panic(70);
    max=0xffff;
    while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x71);
    if (max==0x0) keyboard_panic(70);
    if ((inb(0x60) != 0xfa)) {
        keyboard_panic(996);
    }
    outb(0x80, 0x77);
}
  void
keyboard_panic(status)
  Bit16u status;
{
  bios_printf((2 | 4 | 1), "Keyboard error:%u\n",status);
}
  void
machine_reset()
{
 #asm
;we must check whether 0xFE is set or not
;if it is s3 resume, just jmp back to normal Post Entry
;below port io will prevent s3 resume
  mov al, #0x0f
  out 0x70, al
  in al, 0x71
  cmp al, #0xFE
  jz post
 #endasm
  outb(0x64, 0x60);
  outb(0x60, 0x14);
  outb(0x64, 0xfe);
  bios_printf((2 | 4 | 1), "Couldn't reset the machine\n");
}
  void
clobber_entry_point()
{
    write_word(0xffff, 0x0001, machine_reset);
}
  void
shutdown_status_panic(status)
  Bit16u status;
{
  bios_printf((2 | 4 | 1), "Unimplemented shutdown status: %02x\n",(Bit8u)status);
}
void s3_resume_panic()
{
  bios_printf((2 | 4 | 1), "Returned from s3_resume.\n");
}
void
print_bios_banner()
{
  bios_printf(2, "Bochs"" BIOS - build: %s\n%s\nOptions: ", "06/23/99", bios_cvs_version_string);
  bios_printf(2, "apmbios " "pcibios " "eltorito " "PMM " "\n\n");
}
static char drivetypes[][10]={"", "Floppy","Hard Disk","CD-Rom", "Network"};
static void
init_boot_vectors()
{
  ipl_entry_t e;
  Bit16u count = 0;
  Bit16u ss = get_SS();
  Bit16u ebda_seg = read_word(0x0040, 0x000E);
  memsetb(ebda_seg, 0x0300, 0, 0xff);
  write_word(ebda_seg, 0x0384, 0xFFFF);
  e.type = 0x01; e.flags = 0; e.vector = 0; e.description = 0; e.reserved = 0;
  memcpyb(ebda_seg, 0x0300 + count * sizeof (e), ss, &e, sizeof (e));
  count++;
  e.type = 0x02; e.flags = 0; e.vector = 0; e.description = 0; e.reserved = 0;
  memcpyb(ebda_seg, 0x0300 + count * sizeof (e), ss, &e, sizeof (e));
  count++;
  e.type = 0x03; e.flags = 0; e.vector = 0; e.description = 0; e.reserved = 0;
  memcpyb(ebda_seg, 0x0300 + count * sizeof (e), ss, &e, sizeof (e));
  count++;
  write_word(ebda_seg, 0x0380, count);
  write_word(ebda_seg, 0x0382, 0xffff);
}
static Bit8u
get_boot_vector(i, e)
Bit16u i; ipl_entry_t *e;
{
  Bit16u count;
  Bit16u ss = get_SS();
  Bit16u ebda_seg = read_word(0x0040, 0x000E);
  count = read_word(ebda_seg, 0x0380);
  if (i >= count) return 0;
  memcpyb(ss, e, ebda_seg, 0x0300 + i * sizeof (*e), sizeof (*e));
  return 1;
}
  void
interactive_bootkey()
{
  ipl_entry_t e;
  Bit16u count;
  char description[33];
  Bit8u scan_code;
  Bit8u i;
  Bit16u ss = get_SS();
  Bit16u valid_choice = 0;
  Bit16u ebda_seg = read_word(0x0040, 0x000E);
  bios_printf(2, "\n\nPress F12 for boot menu.\n\n");
  while (check_for_keystroke())
  {
    scan_code = get_keystroke();
    if (scan_code != 0x86)
      continue;
    while (check_for_keystroke())
      get_keystroke();
    bios_printf(2, "Select boot device:\n\n");
    count = read_word(ebda_seg, 0x0380);
    for (i = 0; i < count; i++)
    {
      memcpyb(ss, &e, ebda_seg, 0x0300 + i * sizeof (e), sizeof (e));
      bios_printf(2, "%d. ", i+1);
      switch(e.type)
      {
        case 0x01:
        case 0x02:
        case 0x03:
          bios_printf(2, "%s\n", drivetypes[e.type]);
          break;
        case 0x80:
          bios_printf(2, "%s", drivetypes[4]);
          if (e.description != 0)
          {
            memcpyb(ss, &description, (Bit16u)(e.description >> 16), (Bit16u)(e.description & 0xffff), 32);
            description[32] = 0;
            bios_printf(2, " [%S]", ss, description);
         }
         bios_printf(2, "\n");
         break;
      }
    }
    count++;
    while (!valid_choice) {
      scan_code = get_keystroke();
      if (scan_code == 0x01 || scan_code == 0x58)
      {
        valid_choice = 1;
      }
      else if (scan_code <= count)
      {
        valid_choice = 1;
        scan_code -= 1;
        write_word(ebda_seg, 0x0384, scan_code);
      }
    }
    bios_printf(2, "\n");
    break;
  }
}
void
print_boot_device(type, desc)
  Bit16u type; Bit32u desc;
{
  char description[33];
  Bit16u ss = get_SS();
  if (type == 0x80) type = 0x4;
  if (type == 0 || type > 0x4) bios_printf((2 | 4 | 1), "Bad drive type\n");
  bios_printf(2, "Booting from %s", drivetypes[type]);
  if (type == 4 && desc != 0) {
    memcpyb(ss, &description, (Bit16u)(desc >> 16), (Bit16u)(desc & 0xffff), 32);
    description[32] = 0;
    bios_printf(2, " [%S]", ss, description);
  }
  bios_printf(2, "...\n");
}
  void
print_boot_failure(type, reason)
  Bit16u type; Bit8u reason;
{
  if (type == 0 || type > 0x3) bios_printf((2 | 4 | 1), "Bad drive type\n");
  bios_printf(2, "Boot from %s failed", drivetypes[type]);
  if (type < 4) {
    if (reason==0)
      bios_printf(2, ": not a bootable disk");
    else
      bios_printf(2, ": could not read the boot disk");
  }
  bios_printf(2, "\n\n");
}
  void
print_cdromboot_failure( code )
  Bit16u code;
{
  bios_printf(2 | 4, "CDROM boot failure code : %04x\n",code);
  return;
}
void
nmi_handler_msg()
{
  bios_printf((2 | 4 | 1), "NMI Handler called\n");
}
void
int18_panic_msg()
{
  bios_printf((2 | 4 | 1), "INT18: BOOT FAILURE\n");
}
void
log_bios_start()
{
  bios_printf(4, "%s\n", bios_cvs_version_string);
}
  bx_bool
set_enable_a20(val)
  bx_bool val;
{
  Bit8u oldval;
  oldval = inb(0x92);
  if (val)
    outb(0x92, oldval | 0x02);
  else
    outb(0x92, oldval & 0xfd);
  return((oldval & 0x02) != 0);
}
  void
debugger_on()
{
  outb(0xfedc, 0x01);
}
  void
debugger_off()
{
  outb(0xfedc, 0x00);
}
int
s3_resume()
{
    Bit32u s3_wakeup_vector;
    Bit8u s3_resume_flag;
    s3_resume_flag = read_byte(0x40, 0xb0);
    s3_wakeup_vector = get_s3_waking_vector();
    bios_printf(4, "S3 resume called %x 0x%lx\n", s3_resume_flag, s3_wakeup_vector);
    if (s3_resume_flag != 0xFE || !s3_wakeup_vector)
     return 0;
    write_byte(0x40, 0xb0, 0);
    write_word(0x40, 0xb6, (s3_wakeup_vector & 0xF));
    write_word(0x40, 0xb8, (s3_wakeup_vector >> 4));
    bios_printf(4, "S3 resume jump to %x:%x\n", (s3_wakeup_vector >> 4), (s3_wakeup_vector & 0xF));
 #asm
    jmpf [0x04b6]
 #endasm
    return 1;
}
void ata_init( )
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit8u channel, device;
  for (channel=0; channel<4; channel++) {
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[channel].iface,0x00);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[channel].iobase1,0x0);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[channel].iobase2,0x0);
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[channel].irq,0);
    }
  for (device=0; device<(4*2); device++) {
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type,0x00);
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].device,0x00);
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].removable,0);
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].lock,0);
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].mode,0x00);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].blksize,0);
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].translation,0);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].lchs.heads,0);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].lchs.cylinders,0);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].lchs.spt,0);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].pchs.heads,0);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].pchs.cylinders,0);
    write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].pchs.spt,0);
    write_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_low,0L);
    write_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_high,0L);
    }
  for (device=0; device<(4*2); device++) {
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.hdidmap[device],(4*2));
    write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.cdidmap[device],(4*2));
    }
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.hdcount,0);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.cdcount,0);
}
int await_ide();
static int await_ide(when_done,base,timeout)
  Bit8u when_done;
  Bit16u base;
  Bit16u timeout;
{
  Bit32u time=0,last=0;
  Bit16u status;
  Bit8u result;
  status = inb(base + 7);
  for(;;) {
    status = inb(base+7);
    time++;
    if (when_done == 1)
      result = status & 0x80;
    else if (when_done == 2)
      result = !(status & 0x80);
    else if (when_done == 3)
      result = !(status & 0x80) && (status & 0x08);
    else if (when_done == 4)
      result = !(status & 0x80) && !(status & 0x08);
    else if (when_done == 5)
      result = !(status & 0x80) && (status & 0x40);
    else if (when_done == 0)
      result = 0;
    if (result) return 0;
    if (time>>16 != last)
    {
      last = time >>16;
      ;
    }
    if (status & 0x01)
    {
      ;
      return -1;
    }
    if ((timeout == 0) || ((time>>11) > timeout)) break;
  }
  bios_printf(4, "IDE time out\n");
  return -1;
}
void ata_detect( )
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit8u hdcount, cdcount, device, type;
  Bit8u buffer[0x0200];
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[0].iface,0x00);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[0].iobase1,0x1f0);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[0].iobase2,0x3f0);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[0].irq,14);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[1].iface,0x00);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[1].iobase1,0x170);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[1].iobase2,0x370);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[1].irq,15);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[2].iface,0x00);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[2].iobase1,0x1e8);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[2].iobase2,0x3e0);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[2].irq,12);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[3].iface,0x00);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[3].iobase1,0x168);
  write_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[3].iobase2,0x360);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.channels[3].irq,11);
  hdcount=cdcount=0;
  for(device=0; device<(4*2); device++) {
    Bit16u iobase1, iobase2;
    Bit8u channel, slave, shift;
    Bit8u sc, sn, cl, ch, st;
    channel = device / 2;
    slave = device % 2;
    iobase1 =read_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[channel].iobase1);
    iobase2 =read_word(ebda_seg,&((ebda_data_t *) 0)->ata.channels[channel].iobase2);
    outb(iobase2+6, 0x08 | 0x02);
    outb(iobase1+6, slave ? 0xb0 : 0xa0);
    outb(iobase1+2, 0x55);
    outb(iobase1+3, 0xaa);
    outb(iobase1+2, 0xaa);
    outb(iobase1+3, 0x55);
    outb(iobase1+2, 0x55);
    outb(iobase1+3, 0xaa);
    sc = inb(iobase1+2);
    sn = inb(iobase1+3);
    if ( (sc == 0x55) && (sn == 0xaa) ) {
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type,0x01);
      ata_reset(device);
      outb(iobase1+6, slave ? 0xb0 : 0xa0);
      sc = inb(iobase1+2);
      sn = inb(iobase1+3);
      if ((sc==0x01) && (sn==0x01)) {
        cl = inb(iobase1+4);
        ch = inb(iobase1+5);
        st = inb(iobase1+7);
        if ((cl==0x14) && (ch==0xeb)) {
          write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type,0x03);
        } else if ((cl==0x00) && (ch==0x00) && (st!=0x00)) {
          write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type,0x02);
        } else if ((cl==0xff) && (ch==0xff)) {
          write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type,0x00);
        }
      }
    }
    type=read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type);
    if(type == 0x02) {
      Bit32u sectors_low, sectors_high;
      Bit16u cylinders, heads, spt, blksize;
      Bit8u translation, removable, mode;
      mode = 0x00;
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].device,0xFF);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].mode, 0x00);
      if (ata_cmd_data_in(device,0xEC, 1, 0, 0, 0, 0L, 0L, get_SS(),buffer) !=0 )
        bios_printf((2 | 4 | 1), "ata-detect: Failed to detect ATA device\n");
      removable = (read_byte(get_SS(),buffer+0) & 0x80) ? 1 : 0;
      mode = read_byte(get_SS(),buffer+96) ? 0x01 : 0x00;
      blksize = read_word(get_SS(),buffer+10);
      cylinders = read_word(get_SS(),buffer+(1*2));
      heads = read_word(get_SS(),buffer+(3*2));
      spt = read_word(get_SS(),buffer+(6*2));
      if (read_word(get_SS(),buffer+(83*2)) & (1 << 10)) {
        sectors_low = read_dword(get_SS(),buffer+(100*2));
        sectors_high = read_dword(get_SS(),buffer+(102*2));
      } else {
        sectors_low = read_dword(get_SS(),buffer+(60*2));
        sectors_high = 0;
      }
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].device,0xFF);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].removable, removable);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].mode, mode);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].blksize, blksize);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].pchs.heads, heads);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].pchs.cylinders, cylinders);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].pchs.spt, spt);
      write_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_low, sectors_low);
      write_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_high, sectors_high);
      bios_printf(4, "ata%d-%d: PCHS=%u/%d/%d translation=", channel, slave,cylinders, heads, spt);
      translation = inb_cmos(0x39 + channel/2);
      for (shift=device%4; shift>0; shift--) translation >>= 2;
      translation &= 0x03;
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].translation, translation);
      switch (translation) {
        case 0:
          bios_printf(4, "none");
          break;
        case 1:
          bios_printf(4, "lba");
          break;
        case 2:
          bios_printf(4, "large");
          break;
        case 3:
          bios_printf(4, "r-echs");
          break;
        }
      switch (translation) {
        case 0:
          break;
        case 1:
          spt = 63;
          sectors_low /= 63;
          heads = sectors_low / 1024;
          if (heads>128) heads = 255;
          else if (heads>64) heads = 128;
          else if (heads>32) heads = 64;
          else if (heads>16) heads = 32;
          else heads=16;
          cylinders = sectors_low / heads;
          break;
        case 3:
          if (heads==16) {
            if(cylinders>61439) cylinders=61439;
            heads=15;
            cylinders = (Bit16u)((Bit32u)(cylinders)*16/15);
            }
        case 2:
          while(cylinders > 1024) {
            cylinders >>= 1;
            heads <<= 1;
            if (heads > 127) break;
          }
          break;
        }
      if (cylinders > 1024) cylinders=1024;
      bios_printf(4, " LCHS=%d/%d/%d\n", cylinders, heads, spt);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].lchs.heads, heads);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].lchs.cylinders, cylinders);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].lchs.spt, spt);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.hdidmap[hdcount], device);
      hdcount++;
      }
    if(type == 0x03) {
      Bit8u type, removable, mode;
      Bit16u blksize;
      mode = 0x00;
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].device,0x05);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].mode, 0x00);
      if (ata_cmd_data_in(device,0xA1, 1, 0, 0, 0, 0L, 0L, get_SS(),buffer) != 0)
        bios_printf((2 | 4 | 1), "ata-detect: Failed to detect ATAPI device\n");
      type = read_byte(get_SS(),buffer+1) & 0x1f;
      removable = (read_byte(get_SS(),buffer+0) & 0x80) ? 1 : 0;
      mode = read_byte(get_SS(),buffer+96) ? 0x01 : 0x00;
      blksize = 2048;
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].device, type);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].removable, removable);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].mode, mode);
      write_word(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].blksize, blksize);
      write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.cdidmap[cdcount], device);
      cdcount++;
      }
      {
      Bit32u sizeinmb;
      Bit16u ataversion;
      Bit8u c, i, version, model[41];
      switch (type) {
        case 0x02:
          sizeinmb = (read_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_high) << 21)
            | (read_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_low) >> 11);
        case 0x03:
          ataversion=((Bit16u)(read_byte(get_SS(),buffer+161))<<8)|read_byte(get_SS(),buffer+160);
          for(version=15;version>0;version--) {
            if((ataversion&(1<<version))!=0)
            break;
            }
          for(i=0;i<20;i++){
            write_byte(get_SS(),model+(i*2),read_byte(get_SS(),buffer+(i*2)+54+1));
            write_byte(get_SS(),model+(i*2)+1,read_byte(get_SS(),buffer+(i*2)+54));
          }
          write_byte(get_SS(),model+40,0x00);
          for(i=39;i>0;i--){
            if(read_byte(get_SS(),model+i)==0x20)
              write_byte(get_SS(),model+i,0x00);
            else break;
          }
          if (i>36) {
            write_byte(get_SS(),model+36,0x00);
            for(i=35;i>32;i--){
              write_byte(get_SS(),model+i,0x2E);
            }
          }
          break;
        }
      switch (type) {
        case 0x02:
          bios_printf(2, "ata%d %s: ",channel,slave?" slave":"master");
          i=0; while(c=read_byte(get_SS(),model+i++)) bios_printf(2, "%c",c);
   if (sizeinmb < (1UL<<16))
            bios_printf(2, " ATA-%d Hard-Disk (%4u MBytes)\n", version, (Bit16u)sizeinmb);
   else
            bios_printf(2, " ATA-%d Hard-Disk (%4u GBytes)\n", version, (Bit16u)(sizeinmb>>10));
          break;
        case 0x03:
          bios_printf(2, "ata%d %s: ",channel,slave?" slave":"master");
          i=0; while(c=read_byte(get_SS(),model+i++)) bios_printf(2, "%c",c);
          if(read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].device)==0x05)
            bios_printf(2, " ATAPI-%d CD-Rom/DVD-Rom\n",version);
          else
            bios_printf(2, " ATAPI-%d Device\n",version);
          break;
        case 0x01:
          bios_printf(2, "ata%d %s: Unknown device\n",channel,slave?" slave":"master");
          break;
        }
      }
    }
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.hdcount, hdcount);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->ata.cdcount, cdcount);
  write_byte(0x40,0x75, hdcount);
  bios_printf(2, "\n");
}
void ata_reset(device)
Bit16u device;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit16u iobase1, iobase2;
  Bit8u channel, slave, sn, sc;
  Bit8u type;
  Bit16u max;
  channel = device / 2;
  slave = device % 2;
  iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
  iobase2 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase2);
  outb(iobase2+6, 0x08 | 0x02 | 0x04);
  await_ide(1, iobase1, 20);
  outb(iobase2+6, 0x08 | 0x02);
  type=read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type);
  if (type != 0x00) {
    outb(iobase1+6, slave?0xb0:0xa0);
    sc = inb(iobase1+2);
    sn = inb(iobase1+3);
    if ( (sc==0x01) && (sn==0x01) ) {
      if (type == 0x02)
        await_ide(5, iobase1, 32000u);
      else
        await_ide(2, iobase1, 32000u);
    }
    await_ide(2, iobase1, 32000u);
  }
  outb(iobase2+6, 0x08);
}
Bit16u ata_cmd_non_data()
{return 0;}
Bit16u ata_cmd_data_in(device, command, count, cylinder, head, sector, lba_low, lba_high, segment, offset)
Bit16u device, command, count, cylinder, head, sector, segment, offset;
Bit32u lba_low, lba_high;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit16u iobase1, iobase2, blksize;
  Bit8u channel, slave;
  Bit8u status, current, mode;
  channel = device / 2;
  slave = device % 2;
  iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
  iobase2 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase2);
  mode = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].mode);
  blksize = 0x200;
  if (mode == 0x01) blksize>>=2;
  else blksize>>=1;
  write_word(ebda_seg, &((ebda_data_t *) 0)->ata.trsfsectors,0);
  write_dword(ebda_seg, &((ebda_data_t *) 0)->ata.trsfbytes,0L);
  current = 0;
  status = inb(iobase1 + 7);
  if (status & 0x80) return 1;
  outb(iobase2 + 6, 0x08 | 0x02);
  if (sector == 0) {
    if ((count >= 1 << 8) || lba_high || (lba_low + count >= 1UL << 28)) {
      outb(iobase1 + 1, 0x00);
      outb(iobase1 + 2, (count >> 8) & 0xff);
      outb(iobase1 + 3, lba_low >> 24);
      outb(iobase1 + 4, lba_high & 0xff);
      outb(iobase1 + 5, lba_high >> 8);
      command |= 0x04;
      count &= (1UL << 8) - 1;
      lba_low &= (1UL << 24) - 1;
      }
    sector = (Bit16u) (lba_low & 0x000000ffL);
    cylinder = (Bit16u) ((lba_low>>8) & 0x0000ffffL);
    head = ((Bit16u) ((lba_low>>24) & 0x0000000fL)) | 0x40;
  }
  outb(iobase1 + 1, 0x00);
  outb(iobase1 + 2, count);
  outb(iobase1 + 3, sector);
  outb(iobase1 + 4, cylinder & 0x00ff);
  outb(iobase1 + 5, cylinder >> 8);
  outb(iobase1 + 6, (slave ? 0xb0 : 0xa0) | (Bit8u) head );
  outb(iobase1 + 7, command);
  await_ide(3, iobase1, 32000u);
  status = inb(iobase1 + 7);
  if (status & 0x01) {
    ;
    return 2;
    } else if ( !(status & 0x08) ) {
    ;
    return 3;
  }
 #asm
        sti ;; enable higher priority interrupts
 #endasm
  while (1) {
 #asm
        push bp
        mov bp, sp
        mov di, _ata_cmd_data_in.offset + 2[bp]
        mov ax, _ata_cmd_data_in.segment + 2[bp]
        mov cx, _ata_cmd_data_in.blksize + 2[bp]
        ;; adjust if there will be an overrun. 2K max sector size
        cmp di, #0xf800 ;;
        jbe ata_in_no_adjust
ata_in_adjust:
        sub di, #0x0800 ;; sub 2 kbytes from offset
        add ax, #0x0080 ;; add 2 Kbytes to segment
ata_in_no_adjust:
        mov es, ax ;; segment in es
        mov dx, _ata_cmd_data_in.iobase1 + 2[bp] ;; ATA data read port
        mov ah, _ata_cmd_data_in.mode + 2[bp]
        cmp ah, #0x01
        je ata_in_32
ata_in_16:
        rep
          insw ;; CX words transfered from port(DX) to ES:[DI]
        jmp ata_in_done
ata_in_32:
        rep
          insd ;; CX dwords transfered from port(DX) to ES:[DI]
ata_in_done:
        mov _ata_cmd_data_in.offset + 2[bp], di
        mov _ata_cmd_data_in.segment + 2[bp], es
        pop bp
 #endasm
    current++;
    write_word(ebda_seg, &((ebda_data_t *) 0)->ata.trsfsectors,current);
    count--;
    await_ide(2, iobase1, 32000u);
    status = inb(iobase1 + 7);
    if (count == 0) {
      if ( (status & (0x80 | 0x40 | 0x08 | 0x01) )
          != 0x40 ) {
        ;
        return 4;
        }
      break;
      }
    else {
      if ( (status & (0x80 | 0x40 | 0x08 | 0x01) )
          != (0x40 | 0x08) ) {
        ;
        return 5;
      }
      continue;
    }
  }
  outb(iobase2+6, 0x08);
  return 0;
}
Bit16u ata_cmd_data_out(device, command, count, cylinder, head, sector, lba_low, lba_high, segment, offset)
Bit16u device, command, count, cylinder, head, sector, segment, offset;
Bit32u lba_low, lba_high;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit16u iobase1, iobase2, blksize;
  Bit8u channel, slave;
  Bit8u status, current, mode;
  channel = device / 2;
  slave = device % 2;
  iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
  iobase2 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase2);
  mode = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].mode);
  blksize = 0x200;
  if (mode == 0x01) blksize>>=2;
  else blksize>>=1;
  write_word(ebda_seg, &((ebda_data_t *) 0)->ata.trsfsectors,0);
  write_dword(ebda_seg, &((ebda_data_t *) 0)->ata.trsfbytes,0L);
  current = 0;
  status = inb(iobase1 + 7);
  if (status & 0x80) return 1;
  outb(iobase2 + 6, 0x08 | 0x02);
  if (sector == 0) {
    if ((count >= 1 << 8) || lba_high || (lba_low + count >= 1UL << 28)) {
      outb(iobase1 + 1, 0x00);
      outb(iobase1 + 2, (count >> 8) & 0xff);
      outb(iobase1 + 3, lba_low >> 24);
      outb(iobase1 + 4, lba_high & 0xff);
      outb(iobase1 + 5, lba_high >> 8);
      command |= 0x04;
      count &= (1UL << 8) - 1;
      lba_low &= (1UL << 24) - 1;
      }
    sector = (Bit16u) (lba_low & 0x000000ffL);
    cylinder = (Bit16u) ((lba_low>>8) & 0x0000ffffL);
    head = ((Bit16u) ((lba_low>>24) & 0x0000000fL)) | 0x40;
  }
  outb(iobase1 + 1, 0x00);
  outb(iobase1 + 2, count);
  outb(iobase1 + 3, sector);
  outb(iobase1 + 4, cylinder & 0x00ff);
  outb(iobase1 + 5, cylinder >> 8);
  outb(iobase1 + 6, (slave ? 0xb0 : 0xa0) | (Bit8u) head );
  outb(iobase1 + 7, command);
  await_ide(3, iobase1, 32000u);
  status = inb(iobase1 + 7);
  if (status & 0x01) {
    ;
    return 2;
    } else if ( !(status & 0x08) ) {
    ;
    return 3;
    }
 #asm
        sti ;; enable higher priority interrupts
 #endasm
  while (1) {
 #asm
        push bp
        mov bp, sp
        mov si, _ata_cmd_data_out.offset + 2[bp]
        mov ax, _ata_cmd_data_out.segment + 2[bp]
        mov cx, _ata_cmd_data_out.blksize + 2[bp]
        ;; adjust if there will be an overrun. 2K max sector size
        cmp si, #0xf800 ;;
        jbe ata_out_no_adjust
ata_out_adjust:
        sub si, #0x0800 ;; sub 2 kbytes from offset
        add ax, #0x0080 ;; add 2 Kbytes to segment
ata_out_no_adjust:
        mov es, ax ;; segment in es
        mov dx, _ata_cmd_data_out.iobase1 + 2[bp] ;; ATA data write port
        mov ah, _ata_cmd_data_out.mode + 2[bp]
        cmp ah, #0x01
        je ata_out_32
ata_out_16:
        seg ES
        rep
          outsw ;; CX words transfered from port(DX) to ES:[SI]
        jmp ata_out_done
ata_out_32:
        seg ES
        rep
          outsd ;; CX dwords transfered from port(DX) to ES:[SI]
ata_out_done:
        mov _ata_cmd_data_out.offset + 2[bp], si
        mov _ata_cmd_data_out.segment + 2[bp], es
        pop bp
 #endasm
    current++;
    write_word(ebda_seg, &((ebda_data_t *) 0)->ata.trsfsectors,current);
    count--;
    status = inb(iobase1 + 7);
    if (count == 0) {
      if ( (status & (0x80 | 0x40 | 0x20 | 0x08 | 0x01) )
          != 0x40 ) {
        ;
        return 6;
        }
      break;
      }
    else {
      if ( (status & (0x80 | 0x40 | 0x08 | 0x01) )
          != (0x40 | 0x08) ) {
        ;
        return 7;
      }
      continue;
    }
  }
  outb(iobase2+6, 0x08);
  return 0;
}
Bit16u ata_cmd_packet(device, cmdlen, cmdseg, cmdoff, header, length, inout, bufseg, bufoff)
Bit8u cmdlen,inout;
Bit16u device,cmdseg, cmdoff, bufseg, bufoff;
Bit16u header;
Bit32u length;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit16u iobase1, iobase2;
  Bit16u lcount, lbefore, lafter, count;
  Bit8u channel, slave;
  Bit8u status, mode, lmode;
  Bit32u total, transfer;
  channel = device / 2;
  slave = device % 2;
  if (inout == 0x02) {
    bios_printf(4, "ata_cmd_packet: DATA_OUT not supported yet\n");
    return 1;
    }
  if (header & 1) {
    ;
    return 1;
    }
  iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
  iobase2 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase2);
  mode = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].mode);
  transfer= 0L;
  if (cmdlen < 12) cmdlen=12;
  if (cmdlen > 12) cmdlen=16;
  cmdlen>>=1;
  write_word(ebda_seg, &((ebda_data_t *) 0)->ata.trsfsectors,0);
  write_dword(ebda_seg, &((ebda_data_t *) 0)->ata.trsfbytes,0L);
  status = inb(iobase1 + 7);
  if (status & 0x80) return 2;
  outb(iobase2 + 6, 0x08 | 0x02);
  outb(iobase1 + 1, 0x00);
  outb(iobase1 + 2, 0x00);
  outb(iobase1 + 3, 0x00);
  outb(iobase1 + 4, 0xfff0 & 0x00ff);
  outb(iobase1 + 5, 0xfff0 >> 8);
  outb(iobase1 + 6, slave ? 0xb0 : 0xa0);
  outb(iobase1 + 7, 0xA0);
  await_ide(3, iobase1, 32000u);
  status = inb(iobase1 + 7);
  if (status & 0x01) {
    ;
    return 3;
    } else if ( !(status & 0x08) ) {
    ;
    return 4;
    }
  cmdseg += (cmdoff / 16);
  cmdoff %= 16;
 #asm
      sti ;; enable higher priority interrupts
      push bp
      mov bp, sp
      mov si, _ata_cmd_packet.cmdoff + 2[bp]
      mov ax, _ata_cmd_packet.cmdseg + 2[bp]
      mov cx, _ata_cmd_packet.cmdlen + 2[bp]
      mov es, ax ;; segment in es
      mov dx, _ata_cmd_packet.iobase1 + 2[bp] ;; ATA data write port
      seg ES
      rep
        outsw ;; CX words transfered from port(DX) to ES:[SI]
      pop bp
 #endasm
  if (inout == 0x00) {
    await_ide(2, iobase1, 32000u);
    status = inb(iobase1 + 7);
    }
  else {
        Bit16u loops = 0;
        Bit8u sc;
  while (1) {
      if (loops == 0) {
        status = inb(iobase2 + 6);
        await_ide(3, iobase1, 32000u);
      }
      else
        await_ide(2, iobase1, 32000u);
      loops++;
      status = inb(iobase1 + 7);
      sc = inb(iobase1 + 2);
      if(((inb(iobase1 + 2)&0x7)==0x3) &&
         ((status & (0x40 | 0x01)) == 0x40)) break;
      if (status & 0x01) {
        ;
        return 3;
      }
      bufseg += (bufoff / 16);
      bufoff %= 16;
      lcount = ((Bit16u)(inb(iobase1 + 5))<<8)+inb(iobase1 + 4);
      if(header>lcount) {
         lbefore=lcount;
         header-=lcount;
         lcount=0;
         }
      else {
        lbefore=header;
        header=0;
        lcount-=lbefore;
        }
      if(lcount>length) {
        lafter=lcount-length;
        lcount=length;
        length=0;
        }
      else {
        lafter=0;
        length-=lcount;
        }
      count = lcount;
      ;
      ;
      lmode = mode;
      if (lbefore & 0x03) lmode=0x00;
      if (lcount & 0x03) lmode=0x00;
      if (lafter & 0x03) lmode=0x00;
      if (lcount & 0x01) {
        lcount+=1;
        if ((lafter > 0) && (lafter & 0x01)) {
          lafter-=1;
          }
        }
      if (lmode == 0x01) {
        lcount>>=2; lbefore>>=2; lafter>>=2;
        }
      else {
        lcount>>=1; lbefore>>=1; lafter>>=1;
        }
       ;
 #asm
        push bp
        mov bp, sp
        mov dx, _ata_cmd_packet.iobase1 + 2[bp] ;; ATA data read port
        mov cx, _ata_cmd_packet.lbefore + 2[bp]
        jcxz ata_packet_no_before
        mov ah, _ata_cmd_packet.lmode + 2[bp]
        cmp ah, #0x01
        je ata_packet_in_before_32
ata_packet_in_before_16:
        in ax, dx
        loop ata_packet_in_before_16
        jmp ata_packet_no_before
ata_packet_in_before_32:
        push eax
ata_packet_in_before_32_loop:
        in eax, dx
        loop ata_packet_in_before_32_loop
        pop eax
ata_packet_no_before:
        mov cx, _ata_cmd_packet.lcount + 2[bp]
        jcxz ata_packet_after
        mov di, _ata_cmd_packet.bufoff + 2[bp]
        mov ax, _ata_cmd_packet.bufseg + 2[bp]
        mov es, ax
        mov ah, _ata_cmd_packet.lmode + 2[bp]
        cmp ah, #0x01
        je ata_packet_in_32
ata_packet_in_16:
        rep
          insw ;; CX words transfered tp port(DX) to ES:[DI]
        jmp ata_packet_after
ata_packet_in_32:
        rep
          insd ;; CX dwords transfered to port(DX) to ES:[DI]
ata_packet_after:
        mov cx, _ata_cmd_packet.lafter + 2[bp]
        jcxz ata_packet_done
        mov ah, _ata_cmd_packet.lmode + 2[bp]
        cmp ah, #0x01
        je ata_packet_in_after_32
ata_packet_in_after_16:
        in ax, dx
        loop ata_packet_in_after_16
        jmp ata_packet_done
ata_packet_in_after_32:
        push eax
ata_packet_in_after_32_loop:
        in eax, dx
        loop ata_packet_in_after_32_loop
        pop eax
ata_packet_done:
        pop bp
 #endasm
      bufoff += count;
      transfer += count;
      write_dword(ebda_seg, &((ebda_data_t *) 0)->ata.trsfbytes,transfer);
      }
    }
  if ( (status & (0x80 | 0x40 | 0x20 | 0x08 | 0x01) )
         != 0x40 ) {
    ;
    return 4;
    }
  outb(iobase2+6, 0x08);
  return 0;
}
  Bit16u
atapi_get_sense(device, seg, asc, ascq)
  Bit16u device;
{
  Bit8u atacmd[12];
  Bit8u buffer[18];
  Bit8u i;
  memsetb(get_SS(),atacmd,0,12);
  atacmd[0]=0x03;
  atacmd[4]=sizeof(buffer);
  if (ata_cmd_packet(device, 12, get_SS(), atacmd, 0, 18L, 0x01, get_SS(), buffer) != 0)
    return 0x0002;
  write_byte(seg,asc,buffer[12]);
  write_byte(seg,ascq,buffer[13]);
  return 0;
}
  Bit16u
atapi_is_ready(device)
  Bit16u device;
{
  Bit8u packet[12];
  Bit8u buf[8];
  Bit32u block_len;
  Bit32u sectors;
  Bit32u timeout;
  Bit32u time;
  Bit8u asc, ascq;
  Bit8u in_progress;
  Bit16u ebda_seg = read_word(0x0040,0x000E);
  if (read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type) != 0x03) {
    bios_printf(2, "not implemented for non-ATAPI device\n");
    return -1;
  }
  ;
  memsetb(get_SS(),packet, 0, sizeof packet);
  packet[0] = 0x25;
  timeout = 5000;
  time = 0;
  in_progress = 0;
  while (time < timeout) {
    if (ata_cmd_packet(device, sizeof(packet), get_SS(), packet, 0, 8L, 0x01, get_SS(), buf) == 0)
      goto ok;
    if (atapi_get_sense(device, get_SS(), &asc, &ascq) == 0) {
      if (asc == 0x3a) {
        ;
        return -1;
      }
      if (asc == 0x04 && ascq == 0x01 && !in_progress) {
        bios_printf(2, "Waiting for device to detect medium... ");
        timeout = 30000;
        in_progress = 1;
      }
    }
    time += 100;
  }
  ;
  return -1;
ok:
  block_len = (Bit32u) buf[4] << 24
    | (Bit32u) buf[5] << 16
    | (Bit32u) buf[6] << 8
    | (Bit32u) buf[7] << 0;
  ;
  if (block_len!= 2048 && block_len!= 512)
  {
    bios_printf(2, "Unsupported sector size %u\n", block_len);
    return -1;
  }
  write_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].blksize, block_len);
  sectors = (Bit32u) buf[0] << 24
    | (Bit32u) buf[1] << 16
    | (Bit32u) buf[2] << 8
    | (Bit32u) buf[3] << 0;
  ;
  if (block_len == 2048)
    sectors <<= 2;
  if (sectors != read_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_low))
    bios_printf(2, "%dMB medium detected\n", sectors>>(20-9));
  write_dword(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].sectors_low, sectors);
  return 0;
}
  Bit16u
atapi_is_cdrom(device)
  Bit8u device;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  if (device >= (4*2))
    return 0;
  if (read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].type) != 0x03)
    return 0;
  if (read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.devices[device].device) != 0x05)
    return 0;
  return 1;
}
  void
cdemu_init()
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.active,0x00);
}
  Bit8u
cdemu_isactive()
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  return(read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.active));
}
  Bit8u
cdemu_emulated_drive()
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  return(read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive));
}
static char isotag[6]="CD001";
static char eltorito[24]="EL TORITO SPECIFICATION";
  Bit16u
cdrom_boot()
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit8u atacmd[12], buffer[2048];
  Bit32u lba;
  Bit16u boot_segment, nbsectors, i, error;
  Bit8u device;
  for (device=0; device<(4*2);device++) {
    if (atapi_is_cdrom(device)) break;
    }
  if(device >= (4*2)) return 2;
  if(error = atapi_is_ready(device) != 0)
    bios_printf(4, "ata_is_ready returned %d\n",error);
  memsetb(get_SS(),atacmd,0,12);
  atacmd[0]=0x28;
  atacmd[7]=(0x01 & 0xff00) >> 8;
  atacmd[8]=(0x01 & 0x00ff);
  atacmd[2]=(0x11 & 0xff000000) >> 24;
  atacmd[3]=(0x11 & 0x00ff0000) >> 16;
  atacmd[4]=(0x11 & 0x0000ff00) >> 8;
  atacmd[5]=(0x11 & 0x000000ff);
  if((error = ata_cmd_packet(device, 12, get_SS(), atacmd, 0, 2048L, 0x01, get_SS(), buffer)) != 0)
    return 3;
  if(buffer[0]!=0)return 4;
  for(i=0;i<5;i++){
    if(read_byte(get_SS(),&buffer[1+i])!=read_byte(0xf000,&isotag[i]))return 5;
   }
  for(i=0;i<23;i++)
    if(read_byte(get_SS(),&buffer[7+i])!=read_byte(0xf000,&eltorito[i]))return 6;
  lba=buffer[0x4A]*0x1000000+buffer[0x49]*0x10000+buffer[0x48]*0x100+buffer[0x47];
  memsetb(get_SS(),atacmd,0,12);
  atacmd[0]=0x28;
  atacmd[7]=(0x01 & 0xff00) >> 8;
  atacmd[8]=(0x01 & 0x00ff);
  atacmd[2]=(lba & 0xff000000) >> 24;
  atacmd[3]=(lba & 0x00ff0000) >> 16;
  atacmd[4]=(lba & 0x0000ff00) >> 8;
  atacmd[5]=(lba & 0x000000ff);
  if((error = ata_cmd_packet(device, 12, get_SS(), atacmd, 0, 2048L, 0x01, get_SS(), buffer)) != 0)
    return 7;
  if(buffer[0x00]!=0x01)return 8;
  if(buffer[0x01]!=0x00)return 9;
  if(buffer[0x1E]!=0x55)return 10;
  if(buffer[0x1F]!=0xAA)return 10;
  if(buffer[0x20]!=0x88)return 11;
  write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.media,buffer[0x21]);
  if(buffer[0x21]==0){
    write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive,0xE0);
    }
  else if(buffer[0x21]<4)
    write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive,0x00);
  else
    write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive,0x80);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.controller_index,device/2);
  write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.device_spec,device%2);
  boot_segment=buffer[0x23]*0x100+buffer[0x22];
  if(boot_segment==0x0000)boot_segment=0x07C0;
  write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.load_segment,boot_segment);
  write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.buffer_segment,0x0000);
  nbsectors=buffer[0x27]*0x100+buffer[0x26];
  write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.sector_count,nbsectors);
  lba=buffer[0x2B]*0x1000000+buffer[0x2A]*0x10000+buffer[0x29]*0x100+buffer[0x28];
  write_dword(ebda_seg,&((ebda_data_t *) 0)->cdemu.ilba,lba);
  memsetb(get_SS(),atacmd,0,12);
  atacmd[0]=0x28;
  atacmd[7]=((1+(nbsectors-1)/4) & 0xff00) >> 8;
  atacmd[8]=((1+(nbsectors-1)/4) & 0x00ff);
  atacmd[2]=(lba & 0xff000000) >> 24;
  atacmd[3]=(lba & 0x00ff0000) >> 16;
  atacmd[4]=(lba & 0x0000ff00) >> 8;
  atacmd[5]=(lba & 0x000000ff);
  if((error = ata_cmd_packet(device, 12, get_SS(), atacmd, 0, nbsectors*512L, 0x01, boot_segment,0)) != 0)
    return 12;
  switch(read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.media)) {
    case 0x01:
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.spt,15);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.cylinders,80);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.heads,2);
      break;
    case 0x02:
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.spt,18);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.cylinders,80);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.heads,2);
      break;
    case 0x03:
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.spt,36);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.cylinders,80);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.heads,2);
      break;
    case 0x04:
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.spt,read_byte(boot_segment,446+6)&0x3f);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.cylinders,
              (read_byte(boot_segment,446+6)<<2) + read_byte(boot_segment,446+7) + 1);
      write_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.heads,read_byte(boot_segment,446+5) + 1);
      break;
   }
  if(read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.media)!=0) {
    if(read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive)==0x00)
      write_byte(0x40,0x10,read_byte(0x40,0x10)|0x41);
    else
      write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.hdcount, read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.hdcount) + 1);
   }
  if(read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.media)!=0)
    write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.active,0x01);
  return (read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive)*0x100)+0;
}
  void
int14_function(regs, ds, iret_addr)
  pusha_regs_t regs;
  Bit16u ds;
  iret_addr_t iret_addr;
{
  Bit16u addr,timer,val16;
  Bit8u timeout;
  #asm
  sti
  #endasm
  addr = read_word(0x0040, (regs.u.r16.dx << 1));
  timeout = read_byte(0x0040, 0x007C + regs.u.r16.dx);
  if ((regs.u.r16.dx < 4) && (addr > 0)) {
    switch (regs.u.r8.ah) {
      case 0:
        outb(addr+3, inb(addr+3) | 0x80);
        if (regs.u.r8.al & 0xE0 == 0) {
          outb(addr, 0x17);
          outb(addr+1, 0x04);
        } else {
          val16 = 0x600 >> ((regs.u.r8.al & 0xE0) >> 5);
          outb(addr, val16 & 0xFF);
          outb(addr+1, val16 >> 8);
        }
        outb(addr+3, regs.u.r8.al & 0x1F);
        regs.u.r8.ah = inb(addr+5);
        regs.u.r8.al = inb(addr+6);
        iret_addr.flags.u.r8.flagsl &= 0xfe;
        break;
      case 1:
        timer = read_word(0x0040, 0x006C);
        while (((inb(addr+5) & 0x60) != 0x60) && (timeout)) {
          val16 = read_word(0x0040, 0x006C);
          if (val16 != timer) {
            timer = val16;
            timeout--;
            }
          }
        if (timeout) outb(addr, regs.u.r8.al);
        regs.u.r8.ah = inb(addr+5);
        if (!timeout) regs.u.r8.ah |= 0x80;
        iret_addr.flags.u.r8.flagsl &= 0xfe;
        break;
      case 2:
        timer = read_word(0x0040, 0x006C);
        while (((inb(addr+5) & 0x01) == 0) && (timeout)) {
          val16 = read_word(0x0040, 0x006C);
          if (val16 != timer) {
            timer = val16;
            timeout--;
            }
          }
        if (timeout) {
          regs.u.r8.ah = 0;
          regs.u.r8.al = inb(addr);
        } else {
          regs.u.r8.ah = inb(addr+5);
          }
        iret_addr.flags.u.r8.flagsl &= 0xfe;
        break;
      case 3:
        regs.u.r8.ah = inb(addr+5);
        regs.u.r8.al = inb(addr+6);
        iret_addr.flags.u.r8.flagsl &= 0xfe;
        break;
      default:
        iret_addr.flags.u.r8.flagsl |= 0x01;
      }
  } else {
    iret_addr.flags.u.r8.flagsl |= 0x01;
    }
}
  void
int15_function(regs, ES, DS, FLAGS)
  pusha_regs_t regs;
  Bit16u ES, DS, FLAGS;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  bx_bool prev_a20_enable;
  Bit16u base15_00;
  Bit8u base23_16;
  Bit16u ss;
  Bit16u CX,DX;
  Bit16u bRegister;
  Bit8u irqDisable;
;
  switch (regs.u.r8.ah) {
    case 0x24:
      switch (regs.u.r8.al) {
        case 0x00:
          set_enable_a20(0);
          FLAGS &= 0xfffe;
          regs.u.r8.ah = 0;
          break;
        case 0x01:
          set_enable_a20(1);
          FLAGS &= 0xfffe;
          regs.u.r8.ah = 0;
          break;
        case 0x02:
          regs.u.r8.al = (inb(0x92) >> 1) & 0x01;
          FLAGS &= 0xfffe;
          regs.u.r8.ah = 0;
          break;
        case 0x03:
          FLAGS &= 0xfffe;
          regs.u.r8.ah = 0;
          regs.u.r16.bx = 3;
          break;
        default:
          bios_printf(4, "int15: Func 24h, subfunc %02xh, A20 gate control not supported\n", (unsigned) regs.u.r8.al);
          FLAGS |= 0x0001;
          regs.u.r8.ah = 0x86;
      }
      break;
    case 0x41:
      FLAGS |= 0x0001;
      regs.u.r8.ah = 0x86;
      break;
    case 0x4f:
      FLAGS |= 0x0001;
      break;
    case 0x52:
      FLAGS &= 0xfffe;
      regs.u.r8.ah = 0;
      break;
    case 0x83: {
      if( regs.u.r8.al == 0 ) {
        if( ( read_byte( 0x40, 0xA0 ) & 1 ) == 0 ) {
          write_byte( 0x40, 0xA0, 1 );
          write_word( 0x40, 0x98, ES );
          write_word( 0x40, 0x9A, regs.u.r16.bx );
          write_word( 0x40, 0x9C, regs.u.r16.dx );
          write_word( 0x40, 0x9E, regs.u.r16.cx );
          FLAGS &= 0xfffe;
          irqDisable = inb( 0xA1 );
          outb( 0xA1, irqDisable & 0xFE );
          bRegister = inb_cmos( 0xB );
          outb_cmos( 0xB, bRegister | 0x40 );
        } else {
          ;
          FLAGS |= 0x0001;
          regs.u.r8.ah = 0x86;
        }
      } else if( regs.u.r8.al == 1 ) {
        write_byte( 0x40, 0xA0, 0 );
        FLAGS &= 0xfffe;
        bRegister = inb_cmos( 0xB );
        outb_cmos( 0xB, bRegister & ~0x40 );
      } else {
        ;
        FLAGS |= 0x0001;
        regs.u.r8.ah = 0x86;
        regs.u.r8.al--;
      }
      break;
    }
    case 0x87:
 #asm
  cli
 #endasm
      prev_a20_enable = set_enable_a20(1);
      base15_00 = (ES << 4) + regs.u.r16.si;
      base23_16 = ES >> 12;
      if (base15_00 < (ES<<4))
        base23_16++;
      write_word(ES, regs.u.r16.si+0x08+0, 47);
      write_word(ES, regs.u.r16.si+0x08+2, base15_00);
      write_byte(ES, regs.u.r16.si+0x08+4, base23_16);
      write_byte(ES, regs.u.r16.si+0x08+5, 0x93);
      write_word(ES, regs.u.r16.si+0x08+6, 0x0000);
      write_word(ES, regs.u.r16.si+0x20+0, 0xffff);
      write_word(ES, regs.u.r16.si+0x20+2, 0x0000);
      write_byte(ES, regs.u.r16.si+0x20+4, 0x000f);
      write_byte(ES, regs.u.r16.si+0x20+5, 0x9b);
      write_word(ES, regs.u.r16.si+0x20+6, 0x0000);
      ss = get_SS();
      base15_00 = ss << 4;
      base23_16 = ss >> 12;
      write_word(ES, regs.u.r16.si+0x28+0, 0xffff);
      write_word(ES, regs.u.r16.si+0x28+2, base15_00);
      write_byte(ES, regs.u.r16.si+0x28+4, base23_16);
      write_byte(ES, regs.u.r16.si+0x28+5, 0x93);
      write_word(ES, regs.u.r16.si+0x28+6, 0x0000);
      CX = regs.u.r16.cx;
 #asm
      mov bx, sp
      SEG SS
        mov cx, _int15_function.CX [bx]
      push eax
      xor eax, eax
      mov ds, ax
      mov 0x0469, ss
      mov 0x0467, sp
      SEG ES
        lgdt [si + 0x08]
      SEG CS
        lidt [pmode_IDT_info]
      ;; perhaps do something with IDT here
      ;; set PE bit in CR0
      mov eax, cr0
      or al, #0x01
      mov cr0, eax
      ;; far jump to flush CPU queue after transition to protected mode
      JMP_AP(0x0020, protected_mode)
protected_mode:
      ;; GDT points to valid descriptor table, now load SS, DS, ES
      mov ax, #0x28 ;; 101 000 = 5th descriptor in table, TI=GDT, RPL=00
      mov ss, ax
      mov ax, #0x10 ;; 010 000 = 2nd descriptor in table, TI=GDT, RPL=00
      mov ds, ax
      mov ax, #0x18 ;; 011 000 = 3rd descriptor in table, TI=GDT, RPL=00
      mov es, ax
      xor si, si
      xor di, di
      cld
      rep
        movsw ;; move CX words from DS:SI to ES:DI
      ;; make sure DS and ES limits are 64KB
      mov ax, #0x28
      mov ds, ax
      mov es, ax
      ;; reset PG bit in CR0 ???
      mov eax, cr0
      and al, #0xFE
      mov cr0, eax
      ;; far jump to flush CPU queue after transition to real mode
      JMP_AP(0xf000, real_mode)
real_mode:
      ;; restore IDT to normal real-mode defaults
      SEG CS
        lidt [rmode_IDT_info]
      xor ax, ax
      mov ds, ax
      mov ss, 0x0469
      mov sp, 0x0467
      pop eax
 #endasm
      set_enable_a20(prev_a20_enable);
 #asm
  sti
 #endasm
      regs.u.r8.ah = 0;
      FLAGS &= 0xfffe;
      break;
    case 0x88:
      regs.u.r8.al = inb_cmos(0x30);
      regs.u.r8.ah = inb_cmos(0x31);
      if(regs.u.r16.ax > 0xffc0)
        regs.u.r16.ax = 0xffc0;
      FLAGS &= 0xfffe;
      break;
    case 0x90:
      break;
    case 0x91:
      break;
    case 0xbf:
      bios_printf(4, "*** int 15h function AH=bf not yet supported!\n");
      FLAGS |= 0x0001;
      regs.u.r8.ah = 0x86;
      break;
    case 0xC0:
      FLAGS &= 0xfffe;
      regs.u.r8.ah = 0;
      regs.u.r16.bx = 0xe6f5;
      ES = 0xF000;
      break;
    case 0xc1:
      ES = ebda_seg;
      FLAGS &= 0xfffe;
      break;
    case 0xd8:
      bios_printf(8, "EISA BIOS not present\n");
      FLAGS |= 0x0001;
      regs.u.r8.ah = 0x86;
      break;
    default:
      bios_printf(4, "*** int 15h function AX=%04x, BX=%04x not yet supported!\n", (unsigned) regs.u.r16.ax, (unsigned) regs.u.r16.bx);
      FLAGS |= 0x0001;
      regs.u.r8.ah = 0x86;
      break;
    }
}
  void
int15_function_mouse(regs, ES, DS, FLAGS)
  pusha_regs_t regs;
  Bit16u ES, DS, FLAGS;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit8u mouse_flags_1, mouse_flags_2;
  Bit16u mouse_driver_seg;
  Bit16u mouse_driver_offset;
  Bit8u comm_byte, prev_command_byte;
  Bit8u ret, mouse_data1, mouse_data2, mouse_data3;
;
  switch (regs.u.r8.ah) {
    case 0xC2:
      switch (regs.u.r8.al) {
        case 0:
;
          switch (regs.u.r8.bh) {
            case 0:
;
              inhibit_mouse_int_and_events();
              ret = send_to_mouse_ctrl(0xF5);
              if (ret == 0) {
                ret = get_mouse_data(&mouse_data1);
                if ( (ret == 0) || (mouse_data1 == 0xFA) ) {
                  FLAGS &= 0xfffe;
                  regs.u.r8.ah = 0;
                  return;
                  }
                }
              FLAGS |= 0x0001;
              regs.u.r8.ah = ret;
              return;
              break;
            case 1:
;
              mouse_flags_2 = read_byte(ebda_seg, 0x0027);
              if ( (mouse_flags_2 & 0x80) == 0 ) {
                ;
                FLAGS |= 0x0001;
                regs.u.r8.ah = 5;
                return;
                }
              inhibit_mouse_int_and_events();
              ret = send_to_mouse_ctrl(0xF4);
              if (ret == 0) {
                ret = get_mouse_data(&mouse_data1);
                if ( (ret == 0) && (mouse_data1 == 0xFA) ) {
                  enable_mouse_int_and_events();
                  FLAGS &= 0xfffe;
                  regs.u.r8.ah = 0;
                  return;
                  }
                }
              FLAGS |= 0x0001;
              regs.u.r8.ah = ret;
              return;
            default:
              ;
              FLAGS |= 0x0001;
              regs.u.r8.ah = 1;
              return;
            }
          break;
        case 1:
        case 5:
;
          if (regs.u.r8.al == 5) {
            if (regs.u.r8.bh != 3) {
              FLAGS |= 0x0001;
              regs.u.r8.ah = 0x02;
              return;
            }
            mouse_flags_2 = read_byte(ebda_seg, 0x0027);
            mouse_flags_2 = (mouse_flags_2 & 0x00) | regs.u.r8.bh;
            mouse_flags_1 = 0x00;
            write_byte(ebda_seg, 0x0026, mouse_flags_1);
            write_byte(ebda_seg, 0x0027, mouse_flags_2);
          }
          inhibit_mouse_int_and_events();
          ret = send_to_mouse_ctrl(0xFF);
          if (ret == 0) {
            ret = get_mouse_data(&mouse_data3);
            if (mouse_data3 == 0xfe) {
              FLAGS |= 0x0001;
              return;
            }
            if (mouse_data3 != 0xfa)
              bios_printf((2 | 4 | 1), "Mouse reset returned %02x (should be ack)\n", (unsigned)mouse_data3);
            if ( ret == 0 ) {
              ret = get_mouse_data(&mouse_data1);
              if ( ret == 0 ) {
                ret = get_mouse_data(&mouse_data2);
                if ( ret == 0 ) {
                  enable_mouse_int_and_events();
                  FLAGS &= 0xfffe;
                  regs.u.r8.ah = 0;
                  regs.u.r8.bl = mouse_data1;
                  regs.u.r8.bh = mouse_data2;
                  return;
                  }
                }
              }
            }
          FLAGS |= 0x0001;
          regs.u.r8.ah = ret;
          return;
        case 2:
;
          switch (regs.u.r8.bh) {
            case 0: mouse_data1 = 10; break;
            case 1: mouse_data1 = 20; break;
            case 2: mouse_data1 = 40; break;
            case 3: mouse_data1 = 60; break;
            case 4: mouse_data1 = 80; break;
            case 5: mouse_data1 = 100; break;
            case 6: mouse_data1 = 200; break;
            default: mouse_data1 = 0;
          }
          if (mouse_data1 > 0) {
            ret = send_to_mouse_ctrl(0xF3);
            if (ret == 0) {
              ret = get_mouse_data(&mouse_data2);
              ret = send_to_mouse_ctrl(mouse_data1);
              ret = get_mouse_data(&mouse_data2);
              FLAGS &= 0xfffe;
              regs.u.r8.ah = 0;
            } else {
              FLAGS |= 0x0001;
              regs.u.r8.ah = 0x86;
            }
          } else {
            FLAGS |= 0x0001;
            regs.u.r8.ah = 0x86;
          }
          break;
        case 3:
;
          comm_byte = inhibit_mouse_int_and_events();
          if (regs.u.r8.bh < 4) {
            ret = send_to_mouse_ctrl(0xE8);
            if (ret == 0) {
              ret = get_mouse_data(&mouse_data1);
              if (mouse_data1 != 0xfa)
                bios_printf((2 | 4 | 1), "Mouse status returned %02x (should be ack)\n", (unsigned)mouse_data1);
              ret = send_to_mouse_ctrl(regs.u.r8.bh);
              ret = get_mouse_data(&mouse_data1);
              if (mouse_data1 != 0xfa)
                bios_printf((2 | 4 | 1), "Mouse status returned %02x (should be ack)\n", (unsigned)mouse_data1);
              FLAGS &= 0xfffe;
              regs.u.r8.ah = 0;
            } else {
              FLAGS |= 0x0001;
              regs.u.r8.ah = 0x86;
            }
          } else {
            FLAGS |= 0x0001;
            regs.u.r8.ah = 0x86;
          }
          set_kbd_command_byte(comm_byte);
          break;
        case 4:
;
          inhibit_mouse_int_and_events();
          ret = send_to_mouse_ctrl(0xF2);
          if (ret == 0) {
            ret = get_mouse_data(&mouse_data1);
            ret = get_mouse_data(&mouse_data2);
            FLAGS &= 0xfffe;
            regs.u.r8.ah = 0;
            regs.u.r8.bh = mouse_data2;
          } else {
            FLAGS |= 0x0001;
            regs.u.r8.ah = 0x86;
          }
          break;
        case 6:
;
          switch (regs.u.r8.bh) {
            case 0:
              comm_byte = inhibit_mouse_int_and_events();
              ret = send_to_mouse_ctrl(0xE9);
              if (ret == 0) {
                ret = get_mouse_data(&mouse_data1);
                if (mouse_data1 != 0xfa)
                  bios_printf((2 | 4 | 1), "Mouse status returned %02x (should be ack)\n", (unsigned)mouse_data1);
                if (ret == 0) {
                  ret = get_mouse_data(&mouse_data1);
                  if ( ret == 0 ) {
                    ret = get_mouse_data(&mouse_data2);
                    if ( ret == 0 ) {
                      ret = get_mouse_data(&mouse_data3);
                      if ( ret == 0 ) {
                        FLAGS &= 0xfffe;
                        regs.u.r8.ah = 0;
                        regs.u.r8.bl = mouse_data1;
                        regs.u.r8.cl = mouse_data2;
                        regs.u.r8.dl = mouse_data3;
                        set_kbd_command_byte(comm_byte);
                        return;
                        }
                      }
                    }
                  }
                }
              FLAGS |= 0x0001;
              regs.u.r8.ah = ret;
              set_kbd_command_byte(comm_byte);
              return;
            case 1:
            case 2:
              comm_byte = inhibit_mouse_int_and_events();
              if (regs.u.r8.bh == 1) {
                ret = send_to_mouse_ctrl(0xE6);
              } else {
                ret = send_to_mouse_ctrl(0xE7);
              }
              if (ret == 0) {
                get_mouse_data(&mouse_data1);
                ret = (mouse_data1 != 0xFA);
              }
              if (ret == 0) {
                FLAGS &= 0xfffe;
                regs.u.r8.ah = 0;
              } else {
                FLAGS |= 0x0001;
                regs.u.r8.ah = 0x86;
              }
              set_kbd_command_byte(comm_byte);
              break;
            default:
              bios_printf((2 | 4 | 1), "INT 15h C2 AL=6, BH=%02x\n", (unsigned) regs.u.r8.bh);
            }
          break;
        case 7:
;
          mouse_driver_seg = ES;
          mouse_driver_offset = regs.u.r16.bx;
          write_word(ebda_seg, 0x0022, mouse_driver_offset);
          write_word(ebda_seg, 0x0024, mouse_driver_seg);
          mouse_flags_2 = read_byte(ebda_seg, 0x0027);
          if (mouse_driver_offset == 0 && mouse_driver_seg == 0) {
            if ( (mouse_flags_2 & 0x80) != 0 ) {
              mouse_flags_2 &= ~0x80;
              inhibit_mouse_int_and_events();
              }
            }
          else {
            mouse_flags_2 |= 0x80;
            }
          write_byte(ebda_seg, 0x0027, mouse_flags_2);
          FLAGS &= 0xfffe;
          regs.u.r8.ah = 0;
          break;
        default:
;
          regs.u.r8.ah = 1;
          FLAGS |= 0x0001;
        }
      break;
    default:
      bios_printf(4, "*** int 15h function AX=%04x, BX=%04x not yet supported!\n", (unsigned) regs.u.r16.ax, (unsigned) regs.u.r16.bx);
      FLAGS |= 0x0001;
      regs.u.r8.ah = 0x86;
      break;
    }
}
void set_e820_range(ES, DI, start, end, type)
     Bit16u ES;
     Bit16u DI;
     Bit32u start;
     Bit32u end;
     Bit16u type;
{
    write_word(ES, DI, start);
    write_word(ES, DI+2, start >> 16);
    write_word(ES, DI+4, 0x00);
    write_word(ES, DI+6, 0x00);
    end -= start;
    write_word(ES, DI+8, end);
    write_word(ES, DI+10, end >> 16);
    write_word(ES, DI+12, 0x0000);
    write_word(ES, DI+14, 0x0000);
    write_word(ES, DI+16, type);
    write_word(ES, DI+18, 0x0);
}
  void
int15_function32(regs, ES, DS, FLAGS)
  pushad_regs_t regs;
  Bit16u ES, DS, FLAGS;
{
  Bit32u extended_memory_size=0;
  Bit16u CX,DX;
  Bit16u off, e820_table_size;
  Bit32u base, type, size;
;
  switch (regs.u.r8.ah) {
    case 0x86:
      CX = regs.u.r16.cx;
      DX = regs.u.r16.dx;
 #asm
      sti
      ;; Get the count in eax
      mov bx, sp
SEG SS
      mov ax, _int15_function32.CX [bx]
      shl eax, #16
SEG SS
      mov ax, _int15_function32.DX [bx]
      ;; convert to numbers of 15usec ticks
      mov ebx, #15
      xor edx, edx
      div eax, ebx
      mov ecx, eax
      ;; wait for ecx number of refresh requests
      in al, #0x61
      and al,#0x10
      mov ah, al
      or ecx, ecx
      je int1586_tick_end
int1586_tick:
      in al, #0x61
      and al,#0x10
      cmp al, ah
      je int1586_tick
      mov ah, al
      dec ecx
      jnz int1586_tick
int1586_tick_end:
 #endasm
      break;
    case 0xe8:
        switch(regs.u.r8.al)
        {
       case 0x20: {
            e820_table_size = read_word((Bit16u)(0x000EA100 >> 4), 0x0) * 0x14;
            if (regs.u.r32.edx != 0x534D4150)
                goto int15_unimplemented;
            if ((regs.u.r16.bx / 0x14) * 0x14 == regs.u.r16.bx) {
                if (regs.u.r16.bx + 0x14 <= e820_table_size)
                    memcpyb(ES, regs.u.r16.di,
                            (Bit16u)(0x000EA100 >> 4), 0x8 + regs.u.r16.bx, 0x14);
                regs.u.r32.ebx += 0x14;
                if ((regs.u.r32.ebx + 0x14 - 1) > e820_table_size)
                    regs.u.r32.ebx = 0;
            } else if (regs.u.r16.bx == 1) {
                for (off = 0; off < e820_table_size; off += 0x14) {
                    base = read_dword((Bit16u)(0x000EA100 >> 4), 0x8 + off);
                    type = read_dword((Bit16u)(0x000EA100 >> 4), 0x8 + 0x10 + off);
                    if ((base >= 0x100000) && (type == 1))
                        break;
                }
                if (off == e820_table_size) {
                    FLAGS |= 0x0001;
                    break;
                }
                memcpyb(ES, regs.u.r16.di, (Bit16u)(0x000EA100 >> 4), 0x8 + off, 0x14);
                regs.u.r32.ebx = 0;
            } else {
                goto int15_unimplemented;
            }
            regs.u.r32.eax = 0x534D4150;
            regs.u.r32.ecx = 0x14;
            FLAGS &= 0xfffe;
            break;
        }
        case 0x01: {
            e820_table_size = read_word((Bit16u)(0x000EA100 >> 4), 0x0) * 0x14;
            FLAGS &= 0xfffe;
            regs.u.r8.cl = inb_cmos(0x30);
            regs.u.r8.ch = inb_cmos(0x31);
            if (regs.u.r16.cx > (15*1024))
                regs.u.r16.cx = 15*1024;
            for (off = 0; off < e820_table_size; off += 0x14) {
                base = read_dword((Bit16u)(0x000EA100 >> 4), 0x8 + off);
                type = read_dword((Bit16u)(0x000EA100 >> 4), 0x8 + 0x10 + off);
                if ((base >= 0x100000) && (type == 1))
                    break;
            }
            regs.u.r16.dx = 0;
            if (off != e820_table_size) {
                size = base + read_dword((Bit16u)(0x000EA100 >> 4), 0x8 + 0x8 + off);
                if (size > 0x1000000) {
                    size -= 0x1000000;
                    regs.u.r16.dx = (Bit16u)(size >> 16);
                }
            }
            regs.u.r16.ax = regs.u.r16.cx;
            regs.u.r16.bx = regs.u.r16.dx;
            break;
        }
        default:
            goto int15_unimplemented;
        }
        break;
    int15_unimplemented:
    default:
      bios_printf(4, "*** int 15h function AX=%04x, BX=%04x not yet supported!\n", (unsigned) regs.u.r16.ax, (unsigned) regs.u.r16.bx);
      FLAGS |= 0x0001;
      regs.u.r8.ah = 0x86;
      break;
    }
}
  void
int16_function(DI, SI, BP, SP, BX, DX, CX, AX, FLAGS)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX, FLAGS;
{
  Bit8u scan_code, ascii_code, shift_flags, led_flags, count;
  Bit16u kbd_code, max;
  ;
  shift_flags = read_byte(0x0040, 0x17);
  led_flags = read_byte(0x0040, 0x97);
  if ((((shift_flags >> 4) & 0x07) ^ (led_flags & 0x07)) != 0) {
 #asm
    cli
 #endasm
    outb(0x60, 0xed);
    while ((inb(0x64) & 0x01) == 0) outb(0x80, 0x21);
    if ((inb(0x60) == 0xfa)) {
      led_flags &= 0xf8;
      led_flags |= ((shift_flags >> 4) & 0x07);
      outb(0x60, led_flags & 0x07);
      while ((inb(0x64) & 0x01) == 0) outb(0x80, 0x21);
      inb(0x60);
      write_byte(0x0040, 0x97, led_flags);
    }
 #asm
    sti
 #endasm
  }
  switch (( AX >> 8 )) {
    case 0x00:
      if ( !dequeue_key(&scan_code, &ascii_code, 1) ) {
        bios_printf((2 | 4 | 1), "KBD: int16h: out of keyboard input\n");
        }
      if (scan_code !=0 && ascii_code == 0xF0) ascii_code = 0;
      else if (ascii_code == 0xE0) ascii_code = 0;
      AX = (scan_code << 8) | ascii_code;
      break;
    case 0x01:
      if ( !dequeue_key(&scan_code, &ascii_code, 0) ) {
        FLAGS |= 0x0040;
        return;
        }
      if (scan_code !=0 && ascii_code == 0xF0) ascii_code = 0;
      else if (ascii_code == 0xE0) ascii_code = 0;
      AX = (scan_code << 8) | ascii_code;
      FLAGS &= 0xffbf;
      break;
    case 0x02:
      shift_flags = read_byte(0x0040, 0x17);
      AX = ((AX & 0xff00) | (shift_flags));
      break;
    case 0x05:
      if ( !enqueue_key(( CX >> 8 ), ( CX & 0x00ff )) ) {
        AX = ((AX & 0xff00) | (1));
        }
      else {
        AX = ((AX & 0xff00) | (0));
        }
      break;
    case 0x09:
      AX = ((AX & 0xff00) | (0x30));
      break;
    case 0x0A:
      count = 2;
      kbd_code = 0x0;
      outb(0x60, 0xf2);
      max=0xffff;
      while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x00);
      if (max>0x0) {
        if ((inb(0x60) == 0xfa)) {
          do {
            max=0xffff;
            while ( ((inb(0x64) & 0x01) == 0) && (--max>0) ) outb(0x80, 0x00);
            if (max>0x0) {
              kbd_code >>= 8;
              kbd_code |= (inb(0x60) << 8);
            }
          } while (--count>0);
        }
      }
      BX=kbd_code;
      break;
    case 0x10:
      if ( !dequeue_key(&scan_code, &ascii_code, 1) ) {
        bios_printf((2 | 4 | 1), "KBD: int16h: out of keyboard input\n");
        }
      if (scan_code !=0 && ascii_code == 0xF0) ascii_code = 0;
      AX = (scan_code << 8) | ascii_code;
      break;
    case 0x11:
      if ( !dequeue_key(&scan_code, &ascii_code, 0) ) {
        FLAGS |= 0x0040;
        return;
        }
      if (scan_code !=0 && ascii_code == 0xF0) ascii_code = 0;
      AX = (scan_code << 8) | ascii_code;
      FLAGS &= 0xffbf;
      break;
    case 0x12:
      shift_flags = read_byte(0x0040, 0x17);
      AX = ((AX & 0xff00) | (shift_flags));
      shift_flags = read_byte(0x0040, 0x18) & 0x73;
      shift_flags |= read_byte(0x0040, 0x96) & 0x0c;
      AX = ((AX & 0x00ff) | ((shift_flags) << 8));
      ;
      break;
    case 0x92:
      AX = ((AX & 0x00ff) | ((0x80) << 8));
      break;
    case 0xA2:
      break;
    case 0x6F:
      if (( AX & 0x00ff ) == 0x08)
        AX = ((AX & 0x00ff) | ((0x02) << 8));
    default:
      bios_printf(4, "KBD: unsupported int 16h function %02x\n", ( AX >> 8 ));
    }
}
  unsigned int
dequeue_key(scan_code, ascii_code, incr)
  Bit8u *scan_code;
  Bit8u *ascii_code;
  unsigned int incr;
{
  Bit16u buffer_start, buffer_end, buffer_head, buffer_tail;
  Bit16u ss;
  Bit8u acode, scode;
  buffer_start = read_word(0x0040, 0x0080);
  buffer_end = read_word(0x0040, 0x0082);
  buffer_head = read_word(0x0040, 0x001a);
  buffer_tail = read_word(0x0040, 0x001c);
  if (buffer_head != buffer_tail) {
    ss = get_SS();
    acode = read_byte(0x0040, buffer_head);
    scode = read_byte(0x0040, buffer_head+1);
    write_byte(ss, ascii_code, acode);
    write_byte(ss, scan_code, scode);
    if (incr) {
      buffer_head += 2;
      if (buffer_head >= buffer_end)
        buffer_head = buffer_start;
      write_word(0x0040, 0x001a, buffer_head);
      }
    return(1);
    }
  else {
    return(0);
    }
}
static char panic_msg_keyb_buffer_full[] = "%s: keyboard input buffer full\n";
  Bit8u
inhibit_mouse_int_and_events()
{
  Bit8u command_byte, prev_command_byte;
  if ( inb(0x64) & 0x02 )
    bios_printf((2 | 4 | 1), panic_msg_keyb_buffer_full,"inhibmouse");
  outb(0x64, 0x20);
  while ( (inb(0x64) & 0x01) != 0x01 );
  prev_command_byte = inb(0x60);
  command_byte = prev_command_byte;
  if ( inb(0x64) & 0x02 )
    bios_printf((2 | 4 | 1), panic_msg_keyb_buffer_full,"inhibmouse");
  command_byte &= 0xfd;
  command_byte |= 0x20;
  outb(0x64, 0x60);
  outb(0x60, command_byte);
  return(prev_command_byte);
}
  void
enable_mouse_int_and_events()
{
  Bit8u command_byte;
  if ( inb(0x64) & 0x02 )
    bios_printf((2 | 4 | 1), panic_msg_keyb_buffer_full,"enabmouse");
  outb(0x64, 0x20);
  while ( (inb(0x64) & 0x01) != 0x01 );
  command_byte = inb(0x60);
  if ( inb(0x64) & 0x02 )
    bios_printf((2 | 4 | 1), panic_msg_keyb_buffer_full,"enabmouse");
  command_byte |= 0x02;
  command_byte &= 0xdf;
  outb(0x64, 0x60);
  outb(0x60, command_byte);
}
  Bit8u
send_to_mouse_ctrl(sendbyte)
  Bit8u sendbyte;
{
  Bit8u response;
  if ( inb(0x64) & 0x02 )
    bios_printf((2 | 4 | 1), panic_msg_keyb_buffer_full,"sendmouse");
  outb(0x64, 0xD4);
  outb(0x60, sendbyte);
  return(0);
}
  Bit8u
get_mouse_data(data)
  Bit8u *data;
{
  Bit8u response;
  Bit16u ss;
  while ( (inb(0x64) & 0x21) != 0x21 ) {
    }
  response = inb(0x60);
  ss = get_SS();
  write_byte(ss, data, response);
  return(0);
}
  void
set_kbd_command_byte(command_byte)
  Bit8u command_byte;
{
  if ( inb(0x64) & 0x02 )
    bios_printf((2 | 4 | 1), panic_msg_keyb_buffer_full,"setkbdcomm");
  outb(0x64, 0xD4);
  outb(0x64, 0x60);
  outb(0x60, command_byte);
}
  void
int09_function(DI, SI, BP, SP, BX, DX, CX, AX)
  Bit16u DI, SI, BP, SP, BX, DX, CX, AX;
{
  Bit8u scancode, asciicode, shift_flags;
  Bit8u mf2_flags, mf2_state;
  scancode = ( AX & 0x00ff );
  if (scancode == 0) {
    bios_printf(4, "KBD: int09 handler: AL=0\n");
    return;
    }
  shift_flags = read_byte(0x0040, 0x17);
  mf2_flags = read_byte(0x0040, 0x18);
  mf2_state = read_byte(0x0040, 0x96);
  asciicode = 0;
  switch (scancode) {
    case 0x3a:
      shift_flags ^= 0x40;
      write_byte(0x0040, 0x17, shift_flags);
      mf2_flags |= 0x40;
      write_byte(0x0040, 0x18, mf2_flags);
      break;
    case 0xba:
      mf2_flags &= ~0x40;
      write_byte(0x0040, 0x18, mf2_flags);
      break;
    case 0x2a:
      shift_flags |= 0x02;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xaa:
      shift_flags &= ~0x02;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0x36:
      shift_flags |= 0x01;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xb6:
      shift_flags &= ~0x01;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0x1d:
      if ((mf2_state & 0x01) == 0) {
        shift_flags |= 0x04;
        write_byte(0x0040, 0x17, shift_flags);
        if (mf2_state & 0x02) {
          mf2_state |= 0x04;
          write_byte(0x0040, 0x96, mf2_state);
        } else {
          mf2_flags |= 0x01;
          write_byte(0x0040, 0x18, mf2_flags);
        }
      }
      break;
    case 0x9d:
      if ((mf2_state & 0x01) == 0) {
        shift_flags &= ~0x04;
        write_byte(0x0040, 0x17, shift_flags);
        if (mf2_state & 0x02) {
          mf2_state &= ~0x04;
          write_byte(0x0040, 0x96, mf2_state);
        } else {
          mf2_flags &= ~0x01;
          write_byte(0x0040, 0x18, mf2_flags);
        }
      }
      break;
    case 0x38:
      shift_flags |= 0x08;
      write_byte(0x0040, 0x17, shift_flags);
      if (mf2_state & 0x02) {
        mf2_state |= 0x08;
        write_byte(0x0040, 0x96, mf2_state);
      } else {
        mf2_flags |= 0x02;
        write_byte(0x0040, 0x18, mf2_flags);
      }
      break;
    case 0xb8:
      shift_flags &= ~0x08;
      write_byte(0x0040, 0x17, shift_flags);
      if (mf2_state & 0x02) {
        mf2_state &= ~0x08;
        write_byte(0x0040, 0x96, mf2_state);
      } else {
        mf2_flags &= ~0x02;
        write_byte(0x0040, 0x18, mf2_flags);
      }
      break;
    case 0x45:
      if ((mf2_state & 0x03) == 0) {
        mf2_flags |= 0x20;
        write_byte(0x0040, 0x18, mf2_flags);
        shift_flags ^= 0x20;
        write_byte(0x0040, 0x17, shift_flags);
      }
      break;
    case 0xc5:
      if ((mf2_state & 0x03) == 0) {
        mf2_flags &= ~0x20;
        write_byte(0x0040, 0x18, mf2_flags);
      }
      break;
    case 0x46:
      mf2_flags |= 0x10;
      write_byte(0x0040, 0x18, mf2_flags);
      shift_flags ^= 0x10;
      write_byte(0x0040, 0x17, shift_flags);
      break;
    case 0xc6:
      mf2_flags &= ~0x10;
      write_byte(0x0040, 0x18, mf2_flags);
      break;
    case 0x53:
        if ((shift_flags & 0x0c) == 0x0c)
            machine_reset();
    default:
      if (scancode & 0x80) {
        break;
      }
      if (scancode > 0x58) {
        bios_printf(4, "KBD: int09h_handler(): unknown scancode read: 0x%02x!\n", scancode);
        return;
      }
      if (shift_flags & 0x08) {
        asciicode = scan_to_scanascii[scancode].alt;
        scancode = scan_to_scanascii[scancode].alt >> 8;
      } else if (shift_flags & 0x04) {
        asciicode = scan_to_scanascii[scancode].control;
        scancode = scan_to_scanascii[scancode].control >> 8;
      } else if (((mf2_state & 0x02) > 0) && ((scancode >= 0x47) && (scancode <= 0x53))) {
        asciicode = 0xe0;
        scancode = scan_to_scanascii[scancode].normal >> 8;
      } else if (shift_flags & 0x03) {
        if (shift_flags & scan_to_scanascii[scancode].lock_flags) {
          asciicode = scan_to_scanascii[scancode].normal;
          scancode = scan_to_scanascii[scancode].normal >> 8;
        } else {
          asciicode = scan_to_scanascii[scancode].shift;
          scancode = scan_to_scanascii[scancode].shift >> 8;
        }
      } else {
        if (shift_flags & scan_to_scanascii[scancode].lock_flags) {
          asciicode = scan_to_scanascii[scancode].shift;
          scancode = scan_to_scanascii[scancode].shift >> 8;
        } else {
          asciicode = scan_to_scanascii[scancode].normal;
          scancode = scan_to_scanascii[scancode].normal >> 8;
        }
      }
      if (scancode==0 && asciicode==0) {
        bios_printf(4, "KBD: int09h_handler(): scancode & asciicode are zero?\n");
      }
      enqueue_key(scancode, asciicode);
      break;
  }
  if ((scancode & 0x7f) != 0x1d) {
    mf2_state &= ~0x01;
  }
  mf2_state &= ~0x02;
  write_byte(0x0040, 0x96, mf2_state);
}
  unsigned int
enqueue_key(scan_code, ascii_code)
  Bit8u scan_code, ascii_code;
{
  Bit16u buffer_start, buffer_end, buffer_head, buffer_tail, temp_tail;
  buffer_start = read_word(0x0040, 0x0080);
  buffer_end = read_word(0x0040, 0x0082);
  buffer_head = read_word(0x0040, 0x001A);
  buffer_tail = read_word(0x0040, 0x001C);
  temp_tail = buffer_tail;
  buffer_tail += 2;
  if (buffer_tail >= buffer_end)
    buffer_tail = buffer_start;
  if (buffer_tail == buffer_head) {
    return(0);
    }
   write_byte(0x0040, temp_tail, ascii_code);
   write_byte(0x0040, temp_tail+1, scan_code);
   write_word(0x0040, 0x001C, buffer_tail);
   return(1);
}
  void
int74_function(make_farcall, Z, Y, X, status)
  Bit16u make_farcall, Z, Y, X, status;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit8u in_byte, index, package_count;
  Bit8u mouse_flags_1, mouse_flags_2;
;
  make_farcall = 0;
  in_byte = inb(0x64);
  if ( (in_byte & 0x21) != 0x21 ) {
    return;
    }
  in_byte = inb(0x60);
;
  mouse_flags_1 = read_byte(ebda_seg, 0x0026);
  mouse_flags_2 = read_byte(ebda_seg, 0x0027);
  if ( (mouse_flags_2 & 0x80) != 0x80 ) {
      return;
  }
  package_count = mouse_flags_2 & 0x07;
  index = mouse_flags_1 & 0x07;
  write_byte(ebda_seg, 0x28 + index, in_byte);
  if ( (index+1) >= package_count ) {
;
    status = read_byte(ebda_seg, 0x0028 + 0);
    X = read_byte(ebda_seg, 0x0028 + 1);
    Y = read_byte(ebda_seg, 0x0028 + 2);
    Z = 0;
    mouse_flags_1 = 0;
    if (mouse_flags_2 & 0x80)
      make_farcall = 1;
    }
  else {
    mouse_flags_1++;
    }
  write_byte(ebda_seg, 0x0026, mouse_flags_1);
}
  void
int13_harddisk(EHAX, DS, ES, DI, SI, BP, ELDX, BX, DX, CX, AX, IP, CS, FLAGS)
  Bit16u EHAX, DS, ES, DI, SI, BP, ELDX, BX, DX, CX, AX, IP, CS, FLAGS;
{
  Bit32u lba_low, lba_high;
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit16u cylinder, head, sector;
  Bit16u segment, offset;
  Bit16u npc, nph, npspt, nlc, nlh, nlspt;
  Bit16u size, count;
  Bit8u device, status;
  ;
  write_byte(0x0040, 0x008e, 0);
  if ( (( ELDX & 0x00ff ) < 0x80) || (( ELDX & 0x00ff ) >= 0x80 + (4*2)) ) {
    bios_printf(4, "int13_harddisk: function %02x, ELDL out of range %02x\n", ( AX >> 8 ), ( ELDX & 0x00ff ));
    goto int13_fail;
    }
  device=read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.hdidmap[( ELDX & 0x00ff )-0x80]);
  if (device >= (4*2)) {
    bios_printf(4, "int13_harddisk: function %02x, unmapped device for ELDL=%02x\n", ( AX >> 8 ), ( ELDX & 0x00ff ));
    goto int13_fail;
    }
  switch (( AX >> 8 )) {
    case 0x00:
      ata_reset (device);
      goto int13_success;
      break;
    case 0x01:
      status = read_byte(0x0040, 0x0074);
      AX = ((AX & 0x00ff) | ((status) << 8));
      write_byte(0x0040, 0x0074, 0);
      if (status) goto int13_fail_nostatus;
      else goto int13_success_noah;
      break;
    case 0x02:
    case 0x03:
    case 0x04:
      count = ( AX & 0x00ff );
      cylinder = ( CX >> 8 );
      cylinder |= ( ((Bit16u) ( CX & 0x00ff )) << 2) & 0x300;
      sector = (( CX & 0x00ff ) & 0x3f);
      head = ( DX >> 8 );
      segment = ES;
      offset = BX;
      if ((count > 128) || (count == 0) || (sector == 0)) {
        bios_printf(4, "int13_harddisk: function %02x, parameter out of range!\n",( AX >> 8 ));
        goto int13_fail;
      }
      nlc = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.cylinders);
      nlh = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.heads);
      nlspt = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.spt);
      if( (cylinder >= nlc) || (head >= nlh) || (sector > nlspt )) {
        bios_printf(4, "int13_harddisk: function %02x, parameters out of range %04x/%04x/%04x!\n", ( AX >> 8 ), cylinder, head, sector);
        goto int13_fail;
        }
      if ( ( AX >> 8 ) == 0x04 ) goto int13_success;
      nph = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].pchs.heads);
      npspt = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].pchs.spt);
      if ( (nph != nlh) || (npspt != nlspt)) {
        lba_low = ((((Bit32u)cylinder * (Bit32u)nlh) + (Bit32u)head) * (Bit32u)nlspt) + (Bit32u)sector - 1;
        lba_high = 0;
        sector = 0;
        }
      if ( ( AX >> 8 ) == 0x02 )
        status=ata_cmd_data_in(device, 0x20, count, cylinder, head, sector, lba_low, lba_high, segment, offset);
      else
        status=ata_cmd_data_out(device, 0x30, count, cylinder, head, sector, lba_low, lba_high, segment, offset);
      AX = ((AX & 0xff00) | (read_word(ebda_seg, &((ebda_data_t *) 0)->ata.trsfsectors)));
      if (status != 0) {
        bios_printf(4, "int13_harddisk: function %02x, error %02x !\n",( AX >> 8 ),status);
        AX = ((AX & 0x00ff) | ((0x0c) << 8));
        goto int13_fail_noah;
        }
      goto int13_success;
      break;
    case 0x05:
      bios_printf(4, "format disk track called\n");
      goto int13_success;
      return;
      break;
    case 0x08:
      nlc = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.cylinders);
      nlh = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.heads);
      nlspt = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.spt);
      count = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.hdcount);
      nlc = nlc - 2;
      AX = ((AX & 0xff00) | (0));
      CX = ((CX & 0x00ff) | ((nlc & 0xff) << 8));
      CX = ((CX & 0xff00) | (((nlc >> 2) & 0xc0) | (nlspt & 0x3f)));
      DX = ((DX & 0x00ff) | ((nlh - 1) << 8));
      DX = ((DX & 0xff00) | (count));
      goto int13_success;
      break;
    case 0x10:
      status = inb(read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[device/2].iobase1) + 7);
      if ( (status & ( 0x80 | 0x40 )) == 0x40 ) {
        goto int13_success;
        }
      else {
        AX = ((AX & 0x00ff) | ((0xAA) << 8));
        goto int13_fail_noah;
        }
      break;
    case 0x15:
      nlc = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.cylinders);
      nlh = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.heads);
      nlspt = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lchs.spt);
      lba_low = (Bit32u)(nlc - 1) * (Bit32u)nlh * (Bit32u)nlspt;
      CX = lba_low >> 16;
      DX = lba_low & 0xffff;
      AX = ((AX & 0x00ff) | ((3) << 8));
      goto int13_success_noah;
      break;
    case 0x41:
      BX=0xaa55;
      AX = ((AX & 0x00ff) | ((0x30) << 8));
      CX=0x0007;
      goto int13_success_noah;
      break;
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x47:
      count=read_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->count);
      segment=read_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->segment);
      offset=read_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->offset);
      lba_high=read_dword(DS, SI+(Bit16u)&((int13ext_t *) 0)->lba2);
      if (lba_high > read_dword(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].sectors_high) ) {
        bios_printf(4, "int13_harddisk: function %02x. LBA out of range\n",( AX >> 8 ));
        goto int13_fail;
        }
      lba_low=read_dword(DS, SI+(Bit16u)&((int13ext_t *) 0)->lba1);
      if (lba_high == read_dword(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].sectors_high)
          && lba_low >= read_dword(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].sectors_low) ) {
        bios_printf(4, "int13_harddisk: function %02x. LBA out of range\n",( AX >> 8 ));
        goto int13_fail;
        }
      if (( ( AX >> 8 ) == 0x44 ) || ( ( AX >> 8 ) == 0x47 ))
        goto int13_success;
      if ( ( AX >> 8 ) == 0x42 )
        status=ata_cmd_data_in(device, 0x20, count, 0, 0, 0, lba_low, lba_high, segment, offset);
      else
        status=ata_cmd_data_out(device, 0x30, count, 0, 0, 0, lba_low, lba_high, segment, offset);
      count=read_word(ebda_seg, &((ebda_data_t *) 0)->ata.trsfsectors);
      write_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->count, count);
      if (status != 0) {
        bios_printf(4, "int13_harddisk: function %02x, error %02x !\n",( AX >> 8 ),status);
        AX = ((AX & 0x00ff) | ((0x0c) << 8));
        goto int13_fail_noah;
        }
      goto int13_success;
      break;
    case 0x45:
    case 0x49:
      goto int13_success;
      break;
    case 0x46:
      AX = ((AX & 0x00ff) | ((0xb2) << 8));
      goto int13_fail_noah;
      break;
    case 0x48:
      size=read_word(DS,SI+(Bit16u)&((dpt_t *) 0)->size);
      if(size < 0x1a)
        goto int13_fail;
      if(size >= 0x1a) {
        Bit16u blksize;
        npc = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].pchs.cylinders);
        nph = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].pchs.heads);
        npspt = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].pchs.spt);
        lba_low = read_dword(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].sectors_low);
        lba_high = read_dword(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].sectors_high);
        blksize = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].blksize);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->size, 0x1a);
        if (lba_high || (lba_low/npspt)/nph > 0x3fff)
        {
          write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->infos, 0x00);
          write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->cylinders, 0x3fff);
        }
        else
        {
          write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->infos, 0x02);
          write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->cylinders, (Bit32u)npc);
        }
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->heads, (Bit32u)nph);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->spt, (Bit32u)npspt);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->sector_count1, lba_low);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->sector_count2, lba_high);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->blksize, blksize);
        }
      if(size >= 0x1e) {
        Bit8u channel, dev, irq, mode, checksum, i, translation;
        Bit16u iobase1, iobase2, options;
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->size, 0x1e);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->dpte_segment, ebda_seg);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->dpte_offset, &((ebda_data_t *) 0)->ata.dpte);
        channel = device / 2;
        iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
        iobase2 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase2);
        irq = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].irq);
        mode = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].mode);
        translation = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].translation);
        options = (translation==0?0:1)<<3;
        options |= (1<<4);
        options |= (mode==0x01?1:0)<<7;
        options |= (translation==1?1:0)<<9;
        options |= (translation==3?3:0)<<9;
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.iobase1, iobase1);
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.iobase2, iobase2 + 6);
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.prefix, (0xe | (device % 2))<<4 );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.unused, 0xcb );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.irq, irq );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.blkcount, 1 );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.dma, 0 );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.pio, 0 );
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.options, options);
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.reserved, 0);
        if (size >=0x42)
          write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.revision, 0x11);
        else
          write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.revision, 0x10);
        checksum=0;
        for (i=0; i<15; i++) checksum+=read_byte(ebda_seg, ((Bit8u*)(&((ebda_data_t *) 0)->ata.dpte)) + i);
        checksum = ~checksum;
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.checksum, checksum);
        }
      if(size >= 0x42) {
        Bit8u channel, iface, checksum, i;
        Bit16u iobase1;
        channel = device / 2;
        iface = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iface);
        iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->size, 0x42);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->key, 0xbedd);
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->dpi_length, 0x24);
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->reserved1, 0);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->reserved2, 0);
        if (iface==0x00) {
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[0], 'I');
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[1], 'S');
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[2], 'A');
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[3], 0);
          }
        else {
          }
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[0], 'A');
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[1], 'T');
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[2], 'A');
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[3], 0);
        if (iface==0x00) {
          write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_path[0], iobase1);
          write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_path[2], 0);
          write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_path[4], 0L);
          }
        else {
          }
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[0], device%2);
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[1], 0);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[2], 0);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[4], 0L);
        checksum=0;
        for (i=30; i<64; i++) checksum+=read_byte(DS, SI + i);
        checksum = ~checksum;
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->checksum, checksum);
        }
      goto int13_success;
      break;
    case 0x4e:
      switch (( AX & 0x00ff )) {
        case 0x01:
        case 0x03:
        case 0x04:
        case 0x06:
          goto int13_success;
          break;
        default :
          goto int13_fail;
        }
      break;
    case 0x09:
    case 0x0c:
    case 0x0d:
    case 0x11:
    case 0x14:
      bios_printf(4, "int13_harddisk: function %02xh unimplemented, returns success\n", ( AX >> 8 ));
      goto int13_success;
      break;
    case 0x0a:
    case 0x0b:
    case 0x18:
    case 0x50:
    default:
      bios_printf(4, "int13_harddisk: function %02xh unsupported, returns fail\n", ( AX >> 8 ));
      goto int13_fail;
      break;
    }
int13_fail:
    AX = ((AX & 0x00ff) | ((0x01) << 8));
int13_fail_noah:
    write_byte(0x0040, 0x0074, ( AX >> 8 ));
int13_fail_nostatus:
    FLAGS |= 0x0001;
    return;
int13_success:
    AX = ((AX & 0x00ff) | ((0x00) << 8));
int13_success_noah:
    write_byte(0x0040, 0x0074, 0x00);
    FLAGS &= 0xfffe;
    return;
}
  void
int13_cdrom(EHBX, DS, ES, DI, SI, BP, ELDX, BX, DX, CX, AX, IP, CS, FLAGS)
  Bit16u EHBX, DS, ES, DI, SI, BP, ELDX, BX, DX, CX, AX, IP, CS, FLAGS;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit8u device, status, locks;
  Bit8u atacmd[12];
  Bit32u lba;
  Bit16u count, segment, offset, i, size;
  ;
  write_byte(0x0040, 0x0074, 0x00);
  if( (( ELDX & 0x00ff ) < 0xE0) || (( ELDX & 0x00ff ) >= 0xE0+(4*2)) ) {
    bios_printf(4, "int13_cdrom: function %02x, ELDL out of range %02x\n", ( AX >> 8 ), ( ELDX & 0x00ff ));
    goto int13_fail;
    }
  device=read_byte(ebda_seg,&((ebda_data_t *) 0)->ata.cdidmap[( ELDX & 0x00ff )-0xE0]);
  if (device >= (4*2)) {
    bios_printf(4, "int13_cdrom: function %02x, unmapped device for ELDL=%02x\n", ( AX >> 8 ), ( ELDX & 0x00ff ));
    goto int13_fail;
    }
  switch (( AX >> 8 )) {
    case 0x00:
    case 0x09:
    case 0x0c:
    case 0x0d:
    case 0x10:
    case 0x11:
    case 0x14:
    case 0x16:
      goto int13_success;
      break;
    case 0x03:
    case 0x05:
    case 0x43:
      AX = ((AX & 0x00ff) | ((0x03) << 8));
      goto int13_fail_noah;
      break;
    case 0x01:
      status = read_byte(0x0040, 0x0074);
      AX = ((AX & 0x00ff) | ((status) << 8));
      write_byte(0x0040, 0x0074, 0);
      if (status) goto int13_fail_nostatus;
      else goto int13_success_noah;
      break;
    case 0x15:
      AX = ((AX & 0x00ff) | ((0x02) << 8));
      goto int13_fail_noah;
      break;
    case 0x41:
      BX=0xaa55;
      AX = ((AX & 0x00ff) | ((0x30) << 8));
      CX=0x0007;
      goto int13_success_noah;
      break;
    case 0x42:
    case 0x44:
    case 0x47:
      count=read_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->count);
      segment=read_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->segment);
      offset=read_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->offset);
      lba=read_dword(DS, SI+(Bit16u)&((int13ext_t *) 0)->lba2);
      if (lba != 0L) {
        bios_printf((2 | 4 | 1), "int13_cdrom: function %02x. Can't use 64bits lba\n",( AX >> 8 ));
        goto int13_fail;
        }
      lba=read_dword(DS, SI+(Bit16u)&((int13ext_t *) 0)->lba1);
      if (( ( AX >> 8 ) == 0x44 ) || ( ( AX >> 8 ) == 0x47 ))
        goto int13_success;
      memsetb(get_SS(),atacmd,0,12);
      atacmd[0]=0x28;
      atacmd[7]=(count & 0xff00) >> 8;
      atacmd[8]=(count & 0x00ff);
      atacmd[2]=(lba & 0xff000000) >> 24;
      atacmd[3]=(lba & 0x00ff0000) >> 16;
      atacmd[4]=(lba & 0x0000ff00) >> 8;
      atacmd[5]=(lba & 0x000000ff);
      status = ata_cmd_packet(device, 12, get_SS(), atacmd, 0, count*2048L, 0x01, segment,offset);
      count = (Bit16u)(read_dword(ebda_seg, &((ebda_data_t *) 0)->ata.trsfbytes) >> 11);
      write_word(DS, SI+(Bit16u)&((int13ext_t *) 0)->count, count);
      if (status != 0) {
        bios_printf(4, "int13_cdrom: function %02x, status %02x !\n",( AX >> 8 ),status);
        AX = ((AX & 0x00ff) | ((0x0c) << 8));
        goto int13_fail_noah;
        }
      goto int13_success;
      break;
    case 0x45:
      if (( AX & 0x00ff ) > 2) goto int13_fail;
      locks = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lock);
      switch (( AX & 0x00ff )) {
        case 0 :
          if (locks == 0xff) {
            AX = ((AX & 0x00ff) | ((0xb4) << 8));
            AX = ((AX & 0xff00) | (1));
            goto int13_fail_noah;
            }
          write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lock, ++locks);
          AX = ((AX & 0xff00) | (1));
          break;
        case 1 :
          if (locks == 0x00) {
            AX = ((AX & 0x00ff) | ((0xb0) << 8));
            AX = ((AX & 0xff00) | (0));
            goto int13_fail_noah;
            }
          write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lock, --locks);
          AX = ((AX & 0xff00) | (locks==0?0:1));
          break;
        case 2 :
          AX = ((AX & 0xff00) | (locks==0?0:1));
          break;
        }
      goto int13_success;
      break;
    case 0x46:
      locks = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].lock);
      if (locks != 0) {
        AX = ((AX & 0x00ff) | ((0xb1) << 8));
        goto int13_fail_noah;
        }
      #asm
        push bp
        mov bp, sp
        mov ah, #0x52
        int #0x15
        mov _int13_cdrom.status + 2[bp], ah
        jnc int13_cdrom_rme_end
        mov _int13_cdrom.status, #1
int13_cdrom_rme_end:
        pop bp
      #endasm
      if (status != 0) {
        AX = ((AX & 0x00ff) | ((0xb1) << 8));
        goto int13_fail_noah;
      }
      goto int13_success;
      break;
    case 0x48:
      size = read_word(DS,SI+(Bit16u)&((int13ext_t *) 0)->size);
      if(size < 0x1a)
        goto int13_fail;
      if(size >= 0x1a) {
        Bit16u cylinders, heads, spt, blksize;
        blksize = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].blksize);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->size, 0x1a);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->infos, 0x74);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->cylinders, 0xffffffff);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->heads, 0xffffffff);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->spt, 0xffffffff);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->sector_count1, 0xffffffff);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->sector_count2, 0xffffffff);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->blksize, blksize);
        }
      if(size >= 0x1e) {
        Bit8u channel, dev, irq, mode, checksum, i;
        Bit16u iobase1, iobase2, options;
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->size, 0x1e);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->dpte_segment, ebda_seg);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->dpte_offset, &((ebda_data_t *) 0)->ata.dpte);
        channel = device / 2;
        iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
        iobase2 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase2);
        irq = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].irq);
        mode = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.devices[device].mode);
        options = (1<<4);
        options |= (1<<5);
        options |= (1<<6);
        options |= (mode==0x01?1:0<<7);
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.iobase1, iobase1);
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.iobase2, iobase2 + 6);
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.prefix, (0xe | (device % 2))<<4 );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.unused, 0xcb );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.irq, irq );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.blkcount, 1 );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.dma, 0 );
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.pio, 0 );
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.options, options);
        write_word(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.reserved, 0);
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.revision, 0x11);
        checksum=0;
        for (i=0; i<15; i++) checksum+=read_byte(ebda_seg, ((Bit8u*)(&((ebda_data_t *) 0)->ata.dpte)) + i);
        checksum = ~checksum;
        write_byte(ebda_seg, &((ebda_data_t *) 0)->ata.dpte.checksum, checksum);
        }
      if(size >= 0x42) {
        Bit8u channel, iface, checksum, i;
        Bit16u iobase1;
        channel = device / 2;
        iface = read_byte(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iface);
        iobase1 = read_word(ebda_seg, &((ebda_data_t *) 0)->ata.channels[channel].iobase1);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->size, 0x42);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->key, 0xbedd);
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->dpi_length, 0x24);
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->reserved1, 0);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->reserved2, 0);
        if (iface==0x00) {
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[0], 'I');
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[1], 'S');
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[2], 'A');
          write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->host_bus[3], 0);
          }
        else {
          }
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[0], 'A');
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[1], 'T');
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[2], 'A');
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_type[3], 0);
        if (iface==0x00) {
          write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_path[0], iobase1);
          write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_path[2], 0);
          write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->iface_path[4], 0L);
          }
        else {
          }
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[0], device%2);
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[1], 0);
        write_word(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[2], 0);
        write_dword(DS, SI+(Bit16u)&((dpt_t *) 0)->device_path[4], 0L);
        checksum=0;
        for (i=30; i<64; i++) checksum+=read_byte(DS, SI + i);
        checksum = ~checksum;
        write_byte(DS, SI+(Bit16u)&((dpt_t *) 0)->checksum, checksum);
        }
      goto int13_success;
      break;
    case 0x49:
      AX = ((AX & 0x00ff) | ((06) << 8));
      goto int13_fail_nostatus;
      break;
    case 0x4e:
      switch (( AX & 0x00ff )) {
        case 0x01:
        case 0x03:
        case 0x04:
        case 0x06:
          goto int13_success;
          break;
        default :
          goto int13_fail;
        }
      break;
    case 0x02:
    case 0x04:
    case 0x08:
    case 0x0a:
    case 0x0b:
    case 0x18:
    case 0x50:
    default:
      bios_printf(4, "int13_cdrom: unsupported AH=%02x\n", ( AX >> 8 ));
      goto int13_fail;
      break;
    }
int13_fail:
    AX = ((AX & 0x00ff) | ((0x01) << 8));
int13_fail_noah:
    write_byte(0x0040, 0x0074, ( AX >> 8 ));
int13_fail_nostatus:
    FLAGS |= 0x0001;
    return;
int13_success:
    AX = ((AX & 0x00ff) | ((0x00) << 8));
int13_success_noah:
    write_byte(0x0040, 0x0074, 0x00);
    FLAGS &= 0xfffe;
    return;
}
  void
int13_eltorito(DS, ES, DI, SI, BP, SP, BX, DX, CX, AX, IP, CS, FLAGS)
  Bit16u DS, ES, DI, SI, BP, SP, BX, DX, CX, AX, IP, CS, FLAGS;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  ;
  switch (( AX >> 8 )) {
    case 0x4a:
    case 0x4c:
    case 0x4d:
      bios_printf((2 | 4 | 1), "Int13 eltorito call with AX=%04x. Please report\n",AX);
      goto int13_fail;
      break;
    case 0x4b:
      write_byte(DS,SI+0x00,0x13);
      write_byte(DS,SI+0x01,read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.media));
      write_byte(DS,SI+0x02,read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive));
      write_byte(DS,SI+0x03,read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.controller_index));
      write_dword(DS,SI+0x04,read_dword(ebda_seg,&((ebda_data_t *) 0)->cdemu.ilba));
      write_word(DS,SI+0x08,read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.device_spec));
      write_word(DS,SI+0x0a,read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.buffer_segment));
      write_word(DS,SI+0x0c,read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.load_segment));
      write_word(DS,SI+0x0e,read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.sector_count));
      write_byte(DS,SI+0x10,read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.cylinders));
      write_byte(DS,SI+0x11,read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.spt));
      write_byte(DS,SI+0x12,read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.heads));
      if(( AX & 0x00ff ) == 0x00) {
        write_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.active, 0x00);
        }
      goto int13_success;
      break;
    default:
      bios_printf(4, "int13_eltorito: unsupported AH=%02x\n", ( AX >> 8 ));
      goto int13_fail;
      break;
    }
int13_fail:
    AX = ((AX & 0x00ff) | ((0x01) << 8));
    write_byte(0x0040, 0x0074, ( AX >> 8 ));
    FLAGS |= 0x0001;
    return;
int13_success:
    AX = ((AX & 0x00ff) | ((0x00) << 8));
    write_byte(0x0040, 0x0074, 0x00);
    FLAGS &= 0xfffe;
    return;
}
  void
int13_cdemu(DS, ES, DI, SI, BP, SP, BX, DX, CX, AX, IP, CS, FLAGS)
  Bit16u DS, ES, DI, SI, BP, SP, BX, DX, CX, AX, IP, CS, FLAGS;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit8u device, status;
  Bit16u vheads, vspt, vcylinders;
  Bit16u head, sector, cylinder, nbsectors;
  Bit32u vlba, ilba, slba, elba;
  Bit16u before, segment, offset;
  Bit8u atacmd[12];
  ;
  device = read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.controller_index) * 2;
  device += read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.device_spec);
  write_byte(0x0040, 0x0074, 0x00);
  if( (read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.active) ==0 )
   || (read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.emulated_drive ) != ( DX & 0x00ff ))) {
    bios_printf(4, "int13_cdemu: function %02x, emulation not active for DL= %02x\n", ( AX >> 8 ), ( DX & 0x00ff ));
    goto int13_fail;
    }
  switch (( AX >> 8 )) {
    case 0x00:
    case 0x09:
    case 0x0c:
    case 0x0d:
    case 0x10:
    case 0x11:
    case 0x14:
    case 0x16:
      goto int13_success;
      break;
    case 0x03:
    case 0x05:
      AX = ((AX & 0x00ff) | ((0x03) << 8));
      goto int13_fail_noah;
      break;
    case 0x01:
      status=read_byte(0x0040, 0x0074);
      AX = ((AX & 0x00ff) | ((status) << 8));
      write_byte(0x0040, 0x0074, 0);
      if (status) goto int13_fail_nostatus;
      else goto int13_success_noah;
      break;
    case 0x02:
    case 0x04:
      vspt = read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.spt);
      vcylinders = read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.cylinders);
      vheads = read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.heads);
      ilba = read_dword(ebda_seg,&((ebda_data_t *) 0)->cdemu.ilba);
      sector = ( CX & 0x00ff ) & 0x003f;
      cylinder = (( CX & 0x00ff ) & 0x00c0) << 2 | ( CX >> 8 );
      head = ( DX >> 8 );
      nbsectors = ( AX & 0x00ff );
      segment = ES;
      offset = BX;
      if(nbsectors==0) goto int13_success;
      if ((sector > vspt)
       || (cylinder >= vcylinders)
       || (head >= vheads)) {
        goto int13_fail;
        }
      if (( AX >> 8 ) == 0x04) goto int13_success;
      segment = ES+(BX / 16);
      offset = BX % 16;
      vlba=((((Bit32u)cylinder*(Bit32u)vheads)+(Bit32u)head)*(Bit32u)vspt)+((Bit32u)(sector-1));
      AX = ((AX & 0xff00) | (nbsectors));
      slba = (Bit32u)vlba/4;
      before= (Bit16u)vlba%4;
      elba = (Bit32u)(vlba+nbsectors-1)/4;
      memsetb(get_SS(),atacmd,0,12);
      atacmd[0]=0x28;
      atacmd[7]=((Bit16u)(elba-slba+1) & 0xff00) >> 8;
      atacmd[8]=((Bit16u)(elba-slba+1) & 0x00ff);
      atacmd[2]=(ilba+slba & 0xff000000) >> 24;
      atacmd[3]=(ilba+slba & 0x00ff0000) >> 16;
      atacmd[4]=(ilba+slba & 0x0000ff00) >> 8;
      atacmd[5]=(ilba+slba & 0x000000ff);
      if((status = ata_cmd_packet(device, 12, get_SS(), atacmd, before*512, nbsectors*512L, 0x01, segment,offset)) != 0) {
        bios_printf(4, "int13_cdemu: function %02x, error %02x !\n",( AX >> 8 ),status);
        AX = ((AX & 0x00ff) | ((0x02) << 8));
        AX = ((AX & 0xff00) | (0));
        goto int13_fail_noah;
        }
      goto int13_success;
      break;
    case 0x08:
      vspt=read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.spt);
      vcylinders=read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.cylinders) - 1;
      vheads=read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.vdevice.heads) - 1;
      AX = ((AX & 0xff00) | (0x00));
      BX = ((BX & 0xff00) | (0x00));
      CX = ((CX & 0x00ff) | ((vcylinders & 0xff) << 8));
      CX = ((CX & 0xff00) | ((( vcylinders >> 2) & 0xc0) | ( vspt & 0x3f )));
      DX = ((DX & 0x00ff) | ((vheads) << 8));
      DX = ((DX & 0xff00) | (0x02));
      switch(read_byte(ebda_seg,&((ebda_data_t *) 0)->cdemu.media)) {
        case 0x01: BX = ((BX & 0xff00) | (0x02)); break;
        case 0x02: BX = ((BX & 0xff00) | (0x04)); break;
        case 0x03: BX = ((BX & 0xff00) | (0x06)); break;
        }
 #asm
      push bp
      mov bp, sp
      mov ax, #diskette_param_table2
      mov _int13_cdemu.DI+2[bp], ax
      mov _int13_cdemu.ES+2[bp], cs
      pop bp
 #endasm
      goto int13_success;
      break;
    case 0x15:
      AX = ((AX & 0x00ff) | ((0x03) << 8));
      goto int13_success_noah;
      break;
    case 0x0a:
    case 0x0b:
    case 0x18:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4e:
    case 0x50:
    default:
      bios_printf(4, "int13_cdemu function AH=%02x unsupported, returns fail\n", ( AX >> 8 ));
      goto int13_fail;
      break;
    }
int13_fail:
    AX = ((AX & 0x00ff) | ((0x01) << 8));
int13_fail_noah:
    write_byte(0x0040, 0x0074, ( AX >> 8 ));
int13_fail_nostatus:
    FLAGS |= 0x0001;
    return;
int13_success:
    AX = ((AX & 0x00ff) | ((0x00) << 8));
int13_success_noah:
    write_byte(0x0040, 0x0074, 0x00);
    FLAGS &= 0xfffe;
    return;
}
void floppy_reset_controller()
{
  Bit8u val8;
  val8 = inb(0x03f2);
  outb(0x03f2, val8 & ~0x04);
  outb(0x03f2, val8 | 0x04);
  do {
    val8 = inb(0x3f4);
  } while ( (val8 & 0xc0) != 0x80 );
}
void floppy_prepare_controller(drive)
  Bit16u drive;
{
  Bit8u val8, dor, prev_reset;
  val8 = read_byte(0x0040, 0x003e);
  val8 &= 0x7f;
  write_byte(0x0040, 0x003e, val8);
  prev_reset = inb(0x03f2) & 0x04;
  if (drive)
    dor = 0x20;
  else
    dor = 0x10;
  dor |= 0x0c;
  dor |= drive;
  outb(0x03f2, dor);
  write_byte(0x40,0x40, 37);
  do {
    val8 = inb(0x3f4);
  } while ( (val8 & 0xc0) != 0x80 );
  if (prev_reset == 0) {
 #asm
    sti
 #endasm
    do {
      val8 = read_byte(0x0040, 0x003e);
    } while ( (val8 & 0x80) == 0 );
    val8 &= 0x7f;
 #asm
    cli
 #endasm
    write_byte(0x0040, 0x003e, val8);
  }
}
  bx_bool
floppy_media_known(drive)
  Bit16u drive;
{
  Bit8u val8;
  Bit16u media_state_offset;
  val8 = read_byte(0x0040, 0x003e);
  if (drive)
    val8 >>= 1;
  val8 &= 0x01;
  if (val8 == 0)
    return(0);
  media_state_offset = 0x0090;
  if (drive)
    media_state_offset += 1;
  val8 = read_byte(0x0040, media_state_offset);
  val8 = (val8 >> 4) & 0x01;
  if (val8 == 0)
    return(0);
  return(1);
}
  bx_bool
floppy_media_sense(drive)
  Bit16u drive;
{
  bx_bool retval;
  Bit16u media_state_offset;
  Bit8u drive_type, config_data, media_state;
  if (floppy_drive_recal(drive) == 0) {
    return(0);
    }
  drive_type = inb_cmos(0x10);
  if (drive == 0)
    drive_type >>= 4;
  else
    drive_type &= 0x0f;
  if ( drive_type == 1 ) {
    config_data = 0x00;
    media_state = 0x25;
    retval = 1;
    }
  else if ( drive_type == 2 ) {
    config_data = 0x00;
    media_state = 0x25;
    retval = 1;
    }
  else if ( drive_type == 3 ) {
    config_data = 0x00;
    media_state = 0x17;
    retval = 1;
    }
  else if ( drive_type == 4 ) {
    config_data = 0x00;
    media_state = 0x17;
    retval = 1;
    }
  else if ( drive_type == 5 ) {
    config_data = 0xCC;
    media_state = 0xD7;
    retval = 1;
    }
  else if ( drive_type == 6 ) {
    config_data = 0x00;
    media_state = 0x27;
    retval = 1;
    }
  else if ( drive_type == 7 ) {
    config_data = 0x00;
    media_state = 0x27;
    retval = 1;
    }
  else if ( drive_type == 8 ) {
    config_data = 0x00;
    media_state = 0x27;
    retval = 1;
    }
  else {
    config_data = 0x00;
    media_state = 0x00;
    retval = 0;
    }
  if (drive == 0)
    media_state_offset = 0x90;
  else
    media_state_offset = 0x91;
  write_byte(0x0040, 0x008B, config_data);
  write_byte(0x0040, media_state_offset, media_state);
  return(retval);
}
  bx_bool
floppy_drive_recal(drive)
  Bit16u drive;
{
  Bit8u val8;
  Bit16u curr_cyl_offset;
  floppy_prepare_controller(drive);
  outb(0x03f5, 0x07);
  outb(0x03f5, drive);
 #asm
  sti
 #endasm
  do {
    val8 = (read_byte(0x0040, 0x003e) & 0x80);
  } while ( val8 == 0 );
  val8 = 0;
 #asm
  cli
 #endasm
  val8 = read_byte(0x0040, 0x003e);
  val8 &= 0x7f;
  if (drive) {
    val8 |= 0x02;
    curr_cyl_offset = 0x0095;
  } else {
    val8 |= 0x01;
    curr_cyl_offset = 0x0094;
  }
  write_byte(0x0040, 0x003e, val8);
  write_byte(0x0040, curr_cyl_offset, 0);
  return(1);
}
  bx_bool
floppy_drive_exists(drive)
  Bit16u drive;
{
  Bit8u drive_type;
  drive_type = inb_cmos(0x10);
  if (drive == 0)
    drive_type >>= 4;
  else
    drive_type &= 0x0f;
  if ( drive_type == 0 )
    return(0);
  else
    return(1);
}
  void
int13_diskette_function(DS, ES, DI, SI, BP, ELDX, BX, DX, CX, AX, IP, CS, FLAGS)
  Bit16u DS, ES, DI, SI, BP, ELDX, BX, DX, CX, AX, IP, CS, FLAGS;
{
  Bit8u drive, num_sectors, track, sector, head, status;
  Bit16u base_address, base_count, base_es;
  Bit8u page, mode_register, val8, dor;
  Bit8u return_status[7];
  Bit8u drive_type, num_floppies, ah;
  Bit16u es, last_addr;
  ;
  ah = ( AX >> 8 );
  switch ( ah ) {
    case 0x00:
;
      drive = ( ELDX & 0x00ff );
      if (drive > 1) {
        AX = ((AX & 0x00ff) | ((1) << 8));
        set_diskette_ret_status(1);
        FLAGS |= 0x0001;
        return;
      }
      drive_type = inb_cmos(0x10);
      if (drive == 0)
        drive_type >>= 4;
      else
        drive_type &= 0x0f;
      if (drive_type == 0) {
        AX = ((AX & 0x00ff) | ((0x80) << 8));
        set_diskette_ret_status(0x80);
        FLAGS |= 0x0001;
        return;
      }
      AX = ((AX & 0x00ff) | ((0) << 8));
      set_diskette_ret_status(0);
      FLAGS &= 0xfffe;
      set_diskette_current_cyl(drive, 0);
      return;
    case 0x01:
      FLAGS &= 0xfffe;
      val8 = read_byte(0x0000, 0x0441);
      AX = ((AX & 0x00ff) | ((val8) << 8));
      if (val8) {
        FLAGS |= 0x0001;
      }
      return;
    case 0x02:
    case 0x03:
    case 0x04:
      num_sectors = ( AX & 0x00ff );
      track = ( CX >> 8 );
      sector = ( CX & 0x00ff );
      head = ( DX >> 8 );
      drive = ( ELDX & 0x00ff );
      if ((drive > 1) || (head > 1) || (sector == 0) ||
          (num_sectors == 0) || (num_sectors > 72)) {
        bios_printf(4, "int13_diskette: read/write/verify: parameter out of range\n");
        AX = ((AX & 0x00ff) | ((1) << 8));
        set_diskette_ret_status(1);
        AX = ((AX & 0xff00) | (0));
        FLAGS |= 0x0001;
        return;
      }
      if (floppy_drive_exists(drive) == 0) {
        AX = ((AX & 0x00ff) | ((0x80) << 8));
        set_diskette_ret_status(0x80);
        AX = ((AX & 0xff00) | (0));
        FLAGS |= 0x0001;
        return;
      }
      if (floppy_media_known(drive) == 0) {
        if (floppy_media_sense(drive) == 0) {
          AX = ((AX & 0x00ff) | ((0x0C) << 8));
          set_diskette_ret_status(0x0C);
          AX = ((AX & 0xff00) | (0));
          FLAGS |= 0x0001;
          return;
        }
      }
      if (ah == 0x02) {
        page = (ES >> 12);
        base_es = (ES << 4);
        base_address = base_es + BX;
        if ( base_address < base_es ) {
          page++;
        }
        base_count = (num_sectors * 512) - 1;
        last_addr = base_address + base_count;
        if (last_addr < base_address) {
          AX = ((AX & 0x00ff) | ((0x09) << 8));
          set_diskette_ret_status(0x09);
          AX = ((AX & 0xff00) | (0));
          FLAGS |= 0x0001;
          return;
        }
        ;
        outb(0x000a, 0x06);
  ;
        outb(0x000c, 0x00);
        outb(0x0004, base_address);
        outb(0x0004, base_address>>8);
  ;
        outb(0x000c, 0x00);
        outb(0x0005, base_count);
        outb(0x0005, base_count>>8);
        mode_register = 0x46;
  ;
        outb(0x000b, mode_register);
  ;
        outb(0x0081, page);
  ;
        outb(0x000a, 0x02);
        ;
        outb(0x000a, 0x02);
        floppy_prepare_controller(drive);
        outb(0x03f5, 0xe6);
        outb(0x03f5, (head << 2) | drive);
        outb(0x03f5, track);
        outb(0x03f5, head);
        outb(0x03f5, sector);
        outb(0x03f5, 2);
        outb(0x03f5, sector + num_sectors - 1);
        outb(0x03f5, 0);
        outb(0x03f5, 0xff);
  #asm
        sti
  #endasm
        do {
          val8 = read_byte(0x0040, 0x0040);
          if (val8 == 0) {
            floppy_reset_controller();
            AX = ((AX & 0x00ff) | ((0x80) << 8));
            set_diskette_ret_status(0x80);
            AX = ((AX & 0xff00) | (0));
            FLAGS |= 0x0001;
            return;
          }
          val8 = (read_byte(0x0040, 0x003e) & 0x80);
        } while ( val8 == 0 );
        val8 = 0;
  #asm
        cli
  #endasm
        val8 = read_byte(0x0040, 0x003e);
        val8 &= 0x7f;
        write_byte(0x0040, 0x003e, val8);
        val8 = inb(0x3f4);
        if ( (val8 & 0xc0) != 0xc0 )
          bios_printf((2 | 4 | 1), "int13_diskette: ctrl not ready\n");
        return_status[0] = inb(0x3f5);
        return_status[1] = inb(0x3f5);
        return_status[2] = inb(0x3f5);
        return_status[3] = inb(0x3f5);
        return_status[4] = inb(0x3f5);
        return_status[5] = inb(0x3f5);
        return_status[6] = inb(0x3f5);
        write_byte(0x0040, 0x0042, return_status[0]);
        write_byte(0x0040, 0x0043, return_status[1]);
        write_byte(0x0040, 0x0044, return_status[2]);
        write_byte(0x0040, 0x0045, return_status[3]);
        write_byte(0x0040, 0x0046, return_status[4]);
        write_byte(0x0040, 0x0047, return_status[5]);
        write_byte(0x0040, 0x0048, return_status[6]);
        if ( (return_status[0] & 0xc0) != 0 ) {
          AX = ((AX & 0x00ff) | ((0x20) << 8));
          set_diskette_ret_status(0x20);
          AX = ((AX & 0xff00) | (0));
          FLAGS |= 0x0001;
          return;
        }
        set_diskette_current_cyl(drive, track);
        AX = ((AX & 0x00ff) | ((0x00) << 8));
        FLAGS &= 0xfffe;
        return;
      } else if (ah == 0x03) {
        page = (ES >> 12);
        base_es = (ES << 4);
        base_address = base_es + BX;
        if ( base_address < base_es ) {
          page++;
        }
        base_count = (num_sectors * 512) - 1;
        last_addr = base_address + base_count;
        if (last_addr < base_address) {
          AX = ((AX & 0x00ff) | ((0x09) << 8));
          set_diskette_ret_status(0x09);
          AX = ((AX & 0xff00) | (0));
          FLAGS |= 0x0001;
          return;
        }
        ;
        outb(0x000a, 0x06);
        outb(0x000c, 0x00);
        outb(0x0004, base_address);
        outb(0x0004, base_address>>8);
        outb(0x000c, 0x00);
        outb(0x0005, base_count);
        outb(0x0005, base_count>>8);
        mode_register = 0x4a;
        outb(0x000b, mode_register);
        outb(0x0081, page);
        ;
        outb(0x000a, 0x02);
        floppy_prepare_controller(drive);
        outb(0x03f5, 0xc5);
        outb(0x03f5, (head << 2) | drive);
        outb(0x03f5, track);
        outb(0x03f5, head);
        outb(0x03f5, sector);
        outb(0x03f5, 2);
        outb(0x03f5, sector + num_sectors - 1);
        outb(0x03f5, 0);
        outb(0x03f5, 0xff);
  #asm
        sti
  #endasm
        do {
          val8 = read_byte(0x0040, 0x0040);
          if (val8 == 0) {
            floppy_reset_controller();
            AX = ((AX & 0x00ff) | ((0x80) << 8));
            set_diskette_ret_status(0x80);
            AX = ((AX & 0xff00) | (0));
            FLAGS |= 0x0001;
            return;
          }
          val8 = (read_byte(0x0040, 0x003e) & 0x80);
        } while ( val8 == 0 );
        val8 = 0;
  #asm
        cli
  #endasm
        val8 = read_byte(0x0040, 0x003e);
        val8 &= 0x7f;
        write_byte(0x0040, 0x003e, val8);
        val8 = inb(0x3f4);
        if ( (val8 & 0xc0) != 0xc0 )
          bios_printf((2 | 4 | 1), "int13_diskette: ctrl not ready\n");
        return_status[0] = inb(0x3f5);
        return_status[1] = inb(0x3f5);
        return_status[2] = inb(0x3f5);
        return_status[3] = inb(0x3f5);
        return_status[4] = inb(0x3f5);
        return_status[5] = inb(0x3f5);
        return_status[6] = inb(0x3f5);
        write_byte(0x0040, 0x0042, return_status[0]);
        write_byte(0x0040, 0x0043, return_status[1]);
        write_byte(0x0040, 0x0044, return_status[2]);
        write_byte(0x0040, 0x0045, return_status[3]);
        write_byte(0x0040, 0x0046, return_status[4]);
        write_byte(0x0040, 0x0047, return_status[5]);
        write_byte(0x0040, 0x0048, return_status[6]);
        if ( (return_status[0] & 0xc0) != 0 ) {
          if ( (return_status[1] & 0x02) != 0 ) {
            AX = 0x0300;
            FLAGS |= 0x0001;
            return;
          } else {
            bios_printf((2 | 4 | 1), "int13_diskette_function: read error\n");
          }
        }
        set_diskette_current_cyl(drive, track);
        AX = ((AX & 0x00ff) | ((0x00) << 8));
        FLAGS &= 0xfffe;
        return;
      } else {
        set_diskette_current_cyl(drive, track);
        FLAGS &= 0xfffe;
        AX = ((AX & 0x00ff) | ((0x00) << 8));
        return;
      }
      break;
    case 0x05:
;
      num_sectors = ( AX & 0x00ff );
      track = ( CX >> 8 );
      head = ( DX >> 8 );
      drive = ( ELDX & 0x00ff );
      if ((drive > 1) || (head > 1) || (track > 79) ||
          (num_sectors == 0) || (num_sectors > 18)) {
        AX = ((AX & 0x00ff) | ((1) << 8));
        set_diskette_ret_status(1);
        FLAGS |= 0x0001;
      }
      if (floppy_drive_exists(drive) == 0) {
        AX = ((AX & 0x00ff) | ((0x80) << 8));
        set_diskette_ret_status(0x80);
        FLAGS |= 0x0001;
        return;
      }
      if (floppy_media_known(drive) == 0) {
        if (floppy_media_sense(drive) == 0) {
          AX = ((AX & 0x00ff) | ((0x0C) << 8));
          set_diskette_ret_status(0x0C);
          AX = ((AX & 0xff00) | (0));
          FLAGS |= 0x0001;
          return;
        }
      }
      page = (ES >> 12);
      base_es = (ES << 4);
      base_address = base_es + BX;
      if ( base_address < base_es ) {
        page++;
      }
      base_count = (num_sectors * 4) - 1;
      last_addr = base_address + base_count;
      if (last_addr < base_address) {
        AX = ((AX & 0x00ff) | ((0x09) << 8));
        set_diskette_ret_status(0x09);
        AX = ((AX & 0xff00) | (0));
        FLAGS |= 0x0001;
        return;
      }
      outb(0x000a, 0x06);
      outb(0x000c, 0x00);
      outb(0x0004, base_address);
      outb(0x0004, base_address>>8);
      outb(0x000c, 0x00);
      outb(0x0005, base_count);
      outb(0x0005, base_count>>8);
      mode_register = 0x4a;
      outb(0x000b, mode_register);
      outb(0x0081, page);
      outb(0x000a, 0x02);
      floppy_prepare_controller(drive);
      outb(0x03f5, 0x4d);
      outb(0x03f5, (head << 2) | drive);
      outb(0x03f5, 2);
      outb(0x03f5, num_sectors);
      outb(0x03f5, 0);
      outb(0x03f5, 0xf6);
  #asm
      sti
  #endasm
      do {
        val8 = read_byte(0x0040, 0x0040);
        if (val8 == 0) {
          floppy_reset_controller();
          AX = ((AX & 0x00ff) | ((0x80) << 8));
          set_diskette_ret_status(0x80);
          FLAGS |= 0x0001;
          return;
        }
        val8 = (read_byte(0x0040, 0x003e) & 0x80);
      } while ( val8 == 0 );
      val8 = 0;
  #asm
      cli
  #endasm
      val8 = read_byte(0x0040, 0x003e);
      val8 &= 0x7f;
      write_byte(0x0040, 0x003e, val8);
      val8 = inb(0x3f4);
      if ( (val8 & 0xc0) != 0xc0 )
        bios_printf((2 | 4 | 1), "int13_diskette: ctrl not ready\n");
      return_status[0] = inb(0x3f5);
      return_status[1] = inb(0x3f5);
      return_status[2] = inb(0x3f5);
      return_status[3] = inb(0x3f5);
      return_status[4] = inb(0x3f5);
      return_status[5] = inb(0x3f5);
      return_status[6] = inb(0x3f5);
      write_byte(0x0040, 0x0042, return_status[0]);
      write_byte(0x0040, 0x0043, return_status[1]);
      write_byte(0x0040, 0x0044, return_status[2]);
      write_byte(0x0040, 0x0045, return_status[3]);
      write_byte(0x0040, 0x0046, return_status[4]);
      write_byte(0x0040, 0x0047, return_status[5]);
      write_byte(0x0040, 0x0048, return_status[6]);
      if ( (return_status[0] & 0xc0) != 0 ) {
        if ( (return_status[1] & 0x02) != 0 ) {
          AX = 0x0300;
          FLAGS |= 0x0001;
          return;
        } else {
          bios_printf((2 | 4 | 1), "int13_diskette_function: write error\n");
        }
      }
      AX = ((AX & 0x00ff) | ((0) << 8));
      set_diskette_ret_status(0);
      set_diskette_current_cyl(drive, 0);
      FLAGS &= 0xfffe;
      return;
    case 0x08:
;
      drive = ( ELDX & 0x00ff );
      if (drive > 1) {
        AX = 0;
        BX = 0;
        CX = 0;
        DX = 0;
        ES = 0;
        DI = 0;
        DX = ((DX & 0xff00) | (num_floppies));
        FLAGS |= 0x0001;
        return;
        }
      drive_type = inb_cmos(0x10);
      num_floppies = 0;
      if (drive_type & 0xf0)
        num_floppies++;
      if (drive_type & 0x0f)
        num_floppies++;
      if (drive == 0)
        drive_type >>= 4;
      else
        drive_type &= 0x0f;
      BX = ((BX & 0x00ff) | ((0) << 8));
      BX = ((BX & 0xff00) | (drive_type));
      AX = ((AX & 0x00ff) | ((0) << 8));
      AX = ((AX & 0xff00) | (0));
      DX = ((DX & 0xff00) | (num_floppies));
      switch (drive_type) {
        case 0:
          CX = 0;
          DX = ((DX & 0x00ff) | ((0) << 8));
          break;
        case 1:
          CX = 0x2709;
          DX = ((DX & 0x00ff) | ((1) << 8));
          break;
        case 2:
          CX = 0x4f0f;
          DX = ((DX & 0x00ff) | ((1) << 8));
          break;
        case 3:
          CX = 0x4f09;
          DX = ((DX & 0x00ff) | ((1) << 8));
          break;
        case 4:
          CX = 0x4f12;
          DX = ((DX & 0x00ff) | ((1) << 8));
          break;
        case 5:
          CX = 0x4f24;
          DX = ((DX & 0x00ff) | ((1) << 8));
          break;
        case 6:
          CX = 0x2708;
          DX = ((DX & 0x00ff) | ((0) << 8));
          break;
        case 7:
          CX = 0x2709;
          DX = ((DX & 0x00ff) | ((0) << 8));
          break;
        case 8:
          CX = 0x2708;
          DX = ((DX & 0x00ff) | ((1) << 8));
          break;
        default:
          bios_printf((2 | 4 | 1), "floppy: int13: bad floppy type\n");
        }
 #asm
      push bp
      mov bp, sp
      mov ax, #diskette_param_table2
      mov _int13_diskette_function.DI+2[bp], ax
      mov _int13_diskette_function.ES+2[bp], cs
      pop bp
 #endasm
      FLAGS &= 0xfffe;
      return;
    case 0x15:
;
      drive = ( ELDX & 0x00ff );
      if (drive > 1) {
        AX = ((AX & 0x00ff) | ((0) << 8));
        FLAGS |= 0x0001;
        return;
        }
      drive_type = inb_cmos(0x10);
      if (drive == 0)
        drive_type >>= 4;
      else
        drive_type &= 0x0f;
      FLAGS &= 0xfffe;
      if (drive_type==0) {
        AX = ((AX & 0x00ff) | ((0) << 8));
        }
      else {
        AX = ((AX & 0x00ff) | ((1) << 8));
        }
      return;
    case 0x16:
;
      drive = ( ELDX & 0x00ff );
      if (drive > 1) {
        AX = ((AX & 0x00ff) | ((0x01) << 8));
        set_diskette_ret_status(0x01);
        FLAGS |= 0x0001;
        return;
        }
      AX = ((AX & 0x00ff) | ((0x06) << 8));
      set_diskette_ret_status(0x06);
      FLAGS |= 0x0001;
      return;
    case 0x17:
;
      AX = ((AX & 0x00ff) | ((0x01) << 8));
      set_diskette_ret_status(1);
      FLAGS |= 0x0001;
      return;
    case 0x18:
;
      AX = ((AX & 0x00ff) | ((0x01) << 8));
      set_diskette_ret_status(1);
      FLAGS |= 0x0001;
      return;
    default:
        bios_printf(4, "int13_diskette: unsupported AH=%02x\n", ( AX >> 8 ));
        AX = ((AX & 0x00ff) | ((0x01) << 8));
        set_diskette_ret_status(1);
        FLAGS |= 0x0001;
        return;
    }
}
 void
set_diskette_ret_status(value)
  Bit8u value;
{
  write_byte(0x0040, 0x0041, value);
}
  void
set_diskette_current_cyl(drive, cyl)
  Bit8u drive;
  Bit8u cyl;
{
  if (drive > 1)
    bios_printf((2 | 4 | 1), "set_diskette_current_cyl(): drive > 1\n");
  write_byte(0x0040, 0x0094+drive, cyl);
}
  void
determine_floppy_media(drive)
  Bit16u drive;
{
}
  void
int17_function(regs, ds, iret_addr)
  pusha_regs_t regs;
  Bit16u ds;
  iret_addr_t iret_addr;
{
  Bit16u addr,timeout;
  Bit8u val8;
  #asm
  sti
  #endasm
  addr = read_word(0x0040, (regs.u.r16.dx << 1) + 8);
  if ((regs.u.r8.ah < 3) && (regs.u.r16.dx < 3) && (addr > 0)) {
    timeout = read_byte(0x0040, 0x0078 + regs.u.r16.dx) << 8;
    if (regs.u.r8.ah == 0) {
      outb(addr, regs.u.r8.al);
      val8 = inb(addr+2);
      outb(addr+2, val8 | 0x01);
      #asm
      nop
      #endasm
      outb(addr+2, val8 & ~0x01);
      while (((inb(addr+1) & 0x40) == 0x40) && (timeout)) {
        timeout--;
      }
    }
    if (regs.u.r8.ah == 1) {
      val8 = inb(addr+2);
      outb(addr+2, val8 & ~0x04);
      #asm
      nop
      #endasm
      outb(addr+2, val8 | 0x04);
    }
    val8 = inb(addr+1);
    regs.u.r8.ah = (val8 ^ 0x48);
    if (!timeout) regs.u.r8.ah |= 0x01;
    iret_addr.flags.u.r8.flagsl &= 0xfe;
  } else {
    iret_addr.flags.u.r8.flagsl |= 0x01;
  }
}
void
int18_function(seq_nr)
Bit16u seq_nr;
{
  Bit16u ebda_seg=read_word(0x0040,0x000E);
  Bit16u bootdev;
  Bit8u bootdrv;
  Bit8u bootchk;
  Bit16u bootseg;
  Bit16u bootip;
  Bit16u status;
  Bit16u bootfirst;
  ipl_entry_t e;
  bootdev = inb_cmos(0x3d);
  bootdev |= ((inb_cmos(0x38) & 0xf0) << 4);
  bootdev >>= 4 * seq_nr;
  bootdev &= 0xf;
  bootfirst = read_word(ebda_seg, 0x0384);
  if (bootfirst != 0xFFFF) {
    bootdev = bootfirst;
    write_word(ebda_seg, 0x0384, 0xFFFF);
    write_word(ebda_seg, 0x0382, 0xFFFF);
  } else if (bootdev == 0) {
    bios_printf(2, "\nNo bootable device.\n");
    bios_printf(2, "Powering off in 30 seconds.\n");
 #asm
    sti
    mov cx, #0x01c9
    mov dx, #0xc380
    mov ah, #0x86 ;; INT 15/86: wait CX:DX usec.
    int #0x15
 #endasm
    bios_printf(1, "");
  }
  bootdev -= 1;
  if (get_boot_vector(bootdev, &e) == 0) {
    bios_printf(4, "Invalid boot device (0x%x)\n", bootdev);
    return;
  }
  print_boot_device(e.type, e.description);
  switch(e.type) {
  case 0x01:
  case 0x02:
    bootdrv = (e.type == 0x02) ? 0x80 : 0x00;
    bootseg = 0x07c0;
    status = 0;
 #asm
    push bp
    mov bp, sp
    push ax
    push bx
    push cx
    push dx
    mov dl, _int18_function.bootdrv + 2[bp]
    mov ax, _int18_function.bootseg + 2[bp]
    mov es, ax ;; segment
    xor bx, bx ;; offset
    mov ah, #0x02 ;; function 2, read diskette sector
    mov al, #0x01 ;; read 1 sector
    mov ch, #0x00 ;; track 0
    mov cl, #0x01 ;; sector 1
    mov dh, #0x00 ;; head 0
    int #0x13 ;; read sector
    jnc int19_load_done
    mov ax, #0x0001
    mov _int18_function.status + 2[bp], ax
int19_load_done:
    pop dx
    pop cx
    pop bx
    pop ax
    pop bp
 #endasm
    if (status != 0) {
      print_boot_failure(e.type, 1);
      return;
    }
    if ((e.type != 0x01) || !((inb_cmos(0x38) & 0x01))) {
      if (read_word(bootseg,0x1fe) != 0xaa55) {
        print_boot_failure(e.type, 0);
        return;
      }
    }
    bootip = (bootseg & 0x0fff) << 4;
    bootseg &= 0xf000;
  break;
  case 0x03:
    status = cdrom_boot();
    if ( (status & 0x00ff) !=0 ) {
      print_cdromboot_failure(status);
      print_boot_failure(e.type, 1);
      return;
    }
    bootdrv = (Bit8u)(status>>8);
    bootseg = read_word(ebda_seg,&((ebda_data_t *) 0)->cdemu.load_segment);
    bootip = (bootseg & 0x0fff) << 4;
    bootseg &= 0xf000;
    break;
  case 0x80:
    bootseg = e.vector >> 16;
    bootip = e.vector & 0xffff;
    break;
  default: return;
  }
  bios_printf(4, "Booting from %x:%x\n", bootseg, bootip);
 #asm
    mov bp, sp
    push cs
    push #int18_handler
    ;; Build an iret stack frame that will take us to the boot vector.
    ;; iret pops ip, then cs, then flags, so push them in the opposite order.
    pushf
    mov ax, _int18_function.bootseg + 0[bp]
    push ax
    mov ax, _int18_function.bootip + 0[bp]
    push ax
    ;; Set the magic number in ax and the boot drive in dl.
    mov ax, #0xaa55
    mov dl, _int18_function.bootdrv + 0[bp]
    ;; Zero some of the other registers.
    xor bx, bx
    mov ds, bx
    mov es, bx
    mov bp, bx
    ;; Go!
    iret
 #endasm
}
  void
int1a_function(regs, ds, iret_addr)
  pusha_regs_t regs;
  Bit16u ds;
  iret_addr_t iret_addr;
{
  Bit8u val8;
  ;
  #asm
  sti
  #endasm
  switch (regs.u.r8.ah) {
    case 0:
      #asm
      cli
      #endasm
      regs.u.r16.cx = ((bios_data_t *) 0)->ticks_high;
      regs.u.r16.dx = ((bios_data_t *) 0)->ticks_low;
      regs.u.r8.al = ((bios_data_t *) 0)->midnight_flag;
      ((bios_data_t *) 0)->midnight_flag = 0;
      #asm
      sti
      #endasm
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 1:
      #asm
      cli
      #endasm
      ((bios_data_t *) 0)->ticks_high = regs.u.r16.cx;
      ((bios_data_t *) 0)->ticks_low = regs.u.r16.dx;
      ((bios_data_t *) 0)->midnight_flag = 0;
      #asm
      sti
      #endasm
      regs.u.r8.ah = 0;
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 2:
      if (rtc_updating()) {
        iret_addr.flags.u.r8.flagsl |= 0x01;
        break;
        }
      regs.u.r8.dh = inb_cmos(0x00);
      regs.u.r8.cl = inb_cmos(0x02);
      regs.u.r8.ch = inb_cmos(0x04);
      regs.u.r8.dl = inb_cmos(0x0b) & 0x01;
      regs.u.r8.ah = 0;
      regs.u.r8.al = regs.u.r8.ch;
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 3:
      if (rtc_updating()) {
        init_rtc();
        }
      outb_cmos(0x00, regs.u.r8.dh);
      outb_cmos(0x02, regs.u.r8.cl);
      outb_cmos(0x04, regs.u.r8.ch);
      val8 = (inb_cmos(0x0b) & 0x60) | 0x02 | (regs.u.r8.dl & 0x01);
      outb_cmos(0x0b, val8);
      regs.u.r8.ah = 0;
      regs.u.r8.al = val8;
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 4:
      regs.u.r8.ah = 0;
      if (rtc_updating()) {
        iret_addr.flags.u.r8.flagsl |= 0x01;
        break;
        }
      regs.u.r8.cl = inb_cmos(0x09);
      regs.u.r8.dh = inb_cmos(0x08);
      regs.u.r8.dl = inb_cmos(0x07);
      regs.u.r8.ch = inb_cmos(0x32);
      regs.u.r8.al = regs.u.r8.ch;
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 5:
      if (rtc_updating()) {
        init_rtc();
        iret_addr.flags.u.r8.flagsl |= 0x01;
        break;
        }
      outb_cmos(0x09, regs.u.r8.cl);
      outb_cmos(0x08, regs.u.r8.dh);
      outb_cmos(0x07, regs.u.r8.dl);
      outb_cmos(0x32, regs.u.r8.ch);
      val8 = inb_cmos(0x0b) & 0x7f;
      outb_cmos(0x0b, val8);
      regs.u.r8.ah = 0;
      regs.u.r8.al = val8;
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 6:
      val8 = inb_cmos(0x0b);
      regs.u.r16.ax = 0;
      if (val8 & 0x20) {
        iret_addr.flags.u.r8.flagsl |= 0x01;
        break;
        }
      if (rtc_updating()) {
        init_rtc();
        }
      outb_cmos(0x01, regs.u.r8.dh);
      outb_cmos(0x03, regs.u.r8.cl);
      outb_cmos(0x05, regs.u.r8.ch);
      outb(0xa1, inb(0xa1) & 0xfe);
      outb_cmos(0x0b, (val8 & 0x7f) | 0x20);
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 7:
      val8 = inb_cmos(0x0b);
      outb_cmos(0x0b, val8 & 0x57);
      regs.u.r8.ah = 0;
      regs.u.r8.al = val8;
      iret_addr.flags.u.r8.flagsl &= 0xfe;
      break;
    case 0xb1:
      if (regs.u.r8.bl == 0xff) {
        bios_printf(4, "PCI BIOS: PCI not present\n");
      } else if (regs.u.r8.bl == 0x81) {
        bios_printf(4, "unsupported PCI BIOS function 0x%02x\n", regs.u.r8.al);
      } else if (regs.u.r8.bl == 0x83) {
        bios_printf(4, "bad PCI vendor ID %04x\n", regs.u.r16.dx);
      } else if (regs.u.r8.bl == 0x86) {
        if (regs.u.r8.al == 0x02) {
          bios_printf(4, "PCI device %04x:%04x not found at index %d\n", regs.u.r16.dx, regs.u.r16.cx, regs.u.r16.si);
        } else {
          bios_printf(4, "no PCI device with class code 0x%02x%04x found at index %d\n", regs.u.r8.cl, regs.u.r16.dx, regs.u.r16.si);
        }
      }
      regs.u.r8.ah = regs.u.r8.bl;
      iret_addr.flags.u.r8.flagsl |= 0x01;
      break;
    default:
      iret_addr.flags.u.r8.flagsl |= 0x01;
    }
}
  void
int70_function(regs, ds, iret_addr)
  pusha_regs_t regs;
  Bit16u ds;
  iret_addr_t iret_addr;
{
  Bit8u registerB = 0, registerC = 0;
  registerB = inb_cmos( 0xB );
  registerC = inb_cmos( 0xC );
  if( ( registerB & 0x60 ) != 0 ) {
    if( ( registerC & 0x20 ) != 0 ) {
 #asm
      sti
      int #0x4a
      cli
 #endasm
    }
    if( ( registerC & 0x40 ) != 0 ) {
      if( read_byte( 0x40, 0xA0 ) != 0 ) {
        Bit32u time, toggle;
        time = read_dword( 0x40, 0x9C );
        if( time < 0x3D1 ) {
          Bit16u segment, offset;
          segment = read_word( 0x40, 0x98 );
          offset = read_word( 0x40, 0x9A );
          write_byte( 0x40, 0xA0, 0 );
          outb_cmos( 0xB, registerB & 0x37 );
          write_byte(segment, offset, read_byte(segment, offset) | 0x80 );
        } else {
          time -= 0x3D1;
          write_dword( 0x40, 0x9C, time );
        }
      }
    }
  }
 #asm
  call eoi_both_pics
 #endasm
}
 #asm
;------------------------------------------
;- INT74h : PS/2 mouse hardware interrupt -
;------------------------------------------
int74_handler:
  sti
  pusha
  push ds ;; save DS
  push #0x00 ;; placeholder for status
  push #0x00 ;; placeholder for X
  push #0x00 ;; placeholder for Y
  push #0x00 ;; placeholder for Z
  push #0x00 ;; placeholder for make_far_call boolean
  call _int74_function
  pop cx ;; remove make_far_call from stack
  jcxz int74_done
  ;; make far call to EBDA:0022
  push #0x00
  pop ds
  push 0x040E ;; push 0000:040E (opcodes 0xff, 0x36, 0x0E, 0x04)
  pop ds
  call far ptr[0x22]
int74_done:
  cli
  call eoi_both_pics
  add sp, #8 ;; pop status, x, y, z
  pop ds ;; restore DS
  popa
  iret
;; This will perform an IRET, but will retain value of current CF
;; by altering flags on stack. Better than RETF #02.
iret_modify_cf:
  jc carry_set
  push bp
  mov bp, sp
  and BYTE [bp + 0x06], #0xfe
  pop bp
  iret
carry_set:
  push bp
  mov bp, sp
  or BYTE [bp + 0x06], #0x01
  pop bp
  iret
;----------------------
;- INT13h (relocated) -
;----------------------
;
; int13_relocated is a little bit messed up since I played with it
; I have to rewrite it:
; - call a function that detect which function to call
; - make all called C function get the same parameters list
;
int13_relocated:
  ;; check for an eltorito function
  cmp ah,#0x4a
  jb int13_not_eltorito
  cmp ah,#0x4d
  ja int13_not_eltorito
  pusha
  push es
  push ds
  push ss
  pop ds
  push #int13_out
  jmp _int13_eltorito ;; ELDX not used
int13_not_eltorito:
  push ax
  push bx
  push cx
  push dx
  ;; check if emulation active
  call _cdemu_isactive
  cmp al,#0x00
  je int13_cdemu_inactive
  ;; check if access to the emulated drive
  call _cdemu_emulated_drive
  pop dx
  push dx
  cmp al,dl ;; int13 on emulated drive
  jne int13_nocdemu
  pop dx
  pop cx
  pop bx
  pop ax
  pusha
  push es
  push ds
  push ss
  pop ds
  push #int13_out
  jmp _int13_cdemu ;; ELDX not used
int13_nocdemu:
  and dl,#0xE0 ;; mask to get device class, including cdroms
  cmp al,dl ;; al is 0x00 or 0x80
  jne int13_cdemu_inactive ;; inactive for device class
  pop dx
  pop cx
  pop bx
  pop ax
  push ax
  push cx
  push dx
  push bx
  dec dl ;; real drive is dl - 1
  jmp int13_legacy
int13_cdemu_inactive:
  pop dx
  pop cx
  pop bx
  pop ax
int13_noeltorito:
  push ax
  push cx
  push dx
  push bx
int13_legacy:
  push dx ;; push eltorito value of dx instead of sp
  push bp
  push si
  push di
  push es
  push ds
  push ss
  pop ds
  ;; now the 16-bit registers can be restored with:
  ;; pop ds; pop es; popa; iret
  ;; arguments passed to functions should be
  ;; DS, ES, DI, SI, BP, ELDX, BX, DX, CX, AX, IP, CS, FLAGS
  test dl, #0x80
  jnz int13_notfloppy
  push #int13_out
  jmp _int13_diskette_function
int13_notfloppy:
  cmp dl, #0xE0
  jb int13_notcdrom
  shr ebx, #16
  push bx
  call _int13_cdrom
  pop bx
  shl ebx, #16
  jmp int13_out
int13_notcdrom:
int13_disk:
  ;; int13_harddisk modifies high word of EAX
  shr eax, #16
  push ax
  call _int13_harddisk
  pop ax
  shl eax, #16
int13_out:
  pop ds
  pop es
  popa
  iret
;----------
;- INT18h -
;----------
int18_handler: ;; Boot Failure recovery: try the next device.
  ;; Reset SP and SS
  mov ax, #0x0ffe
  mov sp, ax
  mov ax, #0x9e00
  mov ss, ax
  ;; The first time we do this it will have been set to -1 so
  ;; we will start from device 0.
  xor ax, ax
  mov ds, ax
  mov bx, word ptr [0x40E] ;; EBDA segment
  mov ds, bx ;; Set segment
  mov bx, 0x0382 ;; BX is now the sequence number
  inc bx ;; ++
  mov 0x0382, bx ;; Write it back
  mov ds, ax ;; and reset the segment to zero.
  ;; Call the C code for the next boot device
  push bx
  call _int18_function
  ;; Boot failed: invoke the boot recovery function...
  int #0x18
;----------
;- INT19h -
;----------
int19_relocated: ;; Boot function, relocated
  ;;
  ;; *** Warning: INT 19h resets the whole machine ***
  ;;
  ;; Because PV drivers in HVM guests detach some of the emulated devices,
  ;; it is not safe to do a soft reboot by just dropping to real mode and
  ;; invoking INT 19h -- the boot drives might have disappeared!
  ;; If the user asks for a soft reboot, the only thing we can do is
  ;; reset the whole machine. When it comes back up, the normal BIOS
  ;; boot sequence will start, which is more or less the required behaviour.
  ;;
  ;; Reset SP and SS
  mov ax, #0x0ffe
  mov sp, ax
  mov ax, #0x9e00
  mov ss, ax
  call _machine_reset
;----------
;- INT1Ch -
;----------
int1c_handler: ;; User Timer Tick
  iret
;----------------------
;- POST: Floppy Drive -
;----------------------
floppy_drive_post:
  xor ax, ax
  mov ds, ax
  mov al, #0x00
  mov 0x043e, al ;; drive 0 & 1 uncalibrated, no interrupt has occurred
  mov 0x043f, al ;; diskette motor status: read op, drive0, motors off
  mov 0x0440, al ;; diskette motor timeout counter: not active
  mov 0x0441, al ;; diskette controller status return code
  mov 0x0442, al ;; disk & diskette controller status register 0
  mov 0x0443, al ;; diskette controller status register 1
  mov 0x0444, al ;; diskette controller status register 2
  mov 0x0445, al ;; diskette controller cylinder number
  mov 0x0446, al ;; diskette controller head number
  mov 0x0447, al ;; diskette controller sector number
  mov 0x0448, al ;; diskette controller bytes written
  mov 0x048b, al ;; diskette configuration data
  ;; -----------------------------------------------------------------
  ;; (048F) diskette controller information
  ;;
  mov al, #0x10 ;; get CMOS diskette drive type
  out 0x70, AL
  in AL, 0x71
  mov ah, al ;; save byte to AH
look_drive0:
  shr al, #4 ;; look at top 4 bits for drive 0
  jz f0_missing ;; jump if no drive0
  mov bl, #0x07 ;; drive0 determined, multi-rate, has changed line
  jmp look_drive1
f0_missing:
  mov bl, #0x00 ;; no drive0
look_drive1:
  mov al, ah ;; restore from AH
  and al, #0x0f ;; look at bottom 4 bits for drive 1
  jz f1_missing ;; jump if no drive1
  or bl, #0x70 ;; drive1 determined, multi-rate, has changed line
f1_missing:
                   ;; leave high bits in BL zerod
  mov 0x048f, bl ;; put new val in BDA (diskette controller information)
  ;; -----------------------------------------------------------------
  mov al, #0x00
  mov 0x0490, al ;; diskette 0 media state
  mov 0x0491, al ;; diskette 1 media state
                   ;; diskette 0,1 operational starting state
                   ;; drive type has not been determined,
                   ;; has no changed detection line
  mov 0x0492, al
  mov 0x0493, al
  mov 0x0494, al ;; diskette 0 current cylinder
  mov 0x0495, al ;; diskette 1 current cylinder
  mov al, #0x02
  out #0x0a, al ;; clear DMA-1 channel 2 mask bit
  SET_INT_VECTOR(0x1E, #0xF000, #diskette_param_table2)
  SET_INT_VECTOR(0x40, #0xF000, #int13_diskette)
  SET_INT_VECTOR(0x0E, #0xF000, #int0e_handler) ;; IRQ 6
  ret
;--------------------
;- POST: HARD DRIVE -
;--------------------
; relocated here because the primary POST area isnt big enough.
hard_drive_post:
  mov al, #0x0a ; 0000 1010 = reserved, disable IRQ 14
  mov dx, #0x03f6
  out dx, al
  xor ax, ax
  mov ds, ax
  mov 0x0474, al
  mov 0x0477, al
  mov 0x048c, al
  mov 0x048d, al
  mov 0x048e, al
  mov al, #0x01
  mov 0x0475, al
  mov al, #0xc0
  mov 0x0476, al
  SET_INT_VECTOR(0x13, #0xF000, #int13_handler)
  SET_INT_VECTOR(0x76, #0xF000, #int76_handler)
  ;; INT 41h: hard disk 0 configuration pointer
  ;; INT 46h: hard disk 1 configuration pointer
  SET_INT_VECTOR(0x41, word ptr [0x40E], #0x003D)
  SET_INT_VECTOR(0x46, word ptr [0x40E], #0x004D)
  ;; move disk geometry data from CMOS to EBDA disk parameter table(s)
  mov al, #0x12
  out #0x70, al
  in al, #0x71
  and al, #0xf0
  cmp al, #0xf0
  je post_d0_extended
  jmp check_for_hd1
post_d0_extended:
  mov al, #0x19
  out #0x70, al
  in al, #0x71
  cmp al, #47 ;; decimal 47 - user definable
  je post_d0_type47
  HALT(9038)
post_d0_type47:
  ;; CMOS purpose param table offset
  ;; 1b cylinders low 0
  ;; 1c cylinders high 1
  ;; 1d heads 2
  ;; 1e write pre-comp low 5
  ;; 1f write pre-comp high 6
  ;; 20 retries/bad map/heads>8 8
  ;; 21 landing zone low C
  ;; 22 landing zone high D
  ;; 23 sectors/track E
  xor ax, ax
  mov ds, ax
  mov ax, word ptr [0x40E] ;; EBDA segment
  mov ds, ax
  ;;; Filling EBDA table for hard disk 0.
  mov al, #0x1f
  out #0x70, al
  in al, #0x71
  mov ah, al
  mov al, #0x1e
  out #0x70, al
  in al, #0x71
  mov (0x003d + 0x05), ax ;; write precomp word
  mov al, #0x20
  out #0x70, al
  in al, #0x71
  mov (0x003d + 0x08), al ;; drive control byte
  mov al, #0x22
  out #0x70, al
  in al, #0x71
  mov ah, al
  mov al, #0x21
  out #0x70, al
  in al, #0x71
  mov (0x003d + 0x0C), ax ;; landing zone word
  mov al, #0x1c ;; get cylinders word in AX
  out #0x70, al
  in al, #0x71 ;; high byte
  mov ah, al
  mov al, #0x1b
  out #0x70, al
  in al, #0x71 ;; low byte
  mov bx, ax ;; BX = cylinders
  mov al, #0x1d
  out #0x70, al
  in al, #0x71
  mov cl, al ;; CL = heads
  mov al, #0x23
  out #0x70, al
  in al, #0x71
  mov dl, al ;; DL = sectors
  cmp bx, #1024
  jnbe hd0_post_logical_chs ;; if cylinders > 1024, use translated style CHS
hd0_post_physical_chs:
  ;; no logical CHS mapping used, just physical CHS
  ;; use Standard Fixed Disk Parameter Table (FDPT)
  mov (0x003d + 0x00), bx ;; number of physical cylinders
  mov (0x003d + 0x02), cl ;; number of physical heads
  mov (0x003d + 0x0E), dl ;; number of physical sectors
  jmp check_for_hd1
hd0_post_logical_chs:
  ;; complies with Phoenix style Translated Fixed Disk Parameter Table (FDPT)
  mov (0x003d + 0x09), bx ;; number of physical cylinders
  mov (0x003d + 0x0b), cl ;; number of physical heads
  mov (0x003d + 0x04), dl ;; number of physical sectors
  mov (0x003d + 0x0e), dl ;; number of logical sectors (same)
  mov al, #0xa0
  mov (0x003d + 0x03), al ;; A0h signature, indicates translated table
  cmp bx, #2048
  jnbe hd0_post_above_2048
  ;; 1024 < c <= 2048 cylinders
  shr bx, #0x01
  shl cl, #0x01
  jmp hd0_post_store_logical
hd0_post_above_2048:
  cmp bx, #4096
  jnbe hd0_post_above_4096
  ;; 2048 < c <= 4096 cylinders
  shr bx, #0x02
  shl cl, #0x02
  jmp hd0_post_store_logical
hd0_post_above_4096:
  cmp bx, #8192
  jnbe hd0_post_above_8192
  ;; 4096 < c <= 8192 cylinders
  shr bx, #0x03
  shl cl, #0x03
  jmp hd0_post_store_logical
hd0_post_above_8192:
  ;; 8192 < c <= 16384 cylinders
  shr bx, #0x04
  shl cl, #0x04
hd0_post_store_logical:
  mov (0x003d + 0x00), bx ;; number of physical cylinders
  mov (0x003d + 0x02), cl ;; number of physical heads
  ;; checksum
  mov cl, #0x0f ;; repeat count
  mov si, #0x003d ;; offset to disk0 FDPT
  mov al, #0x00 ;; sum
hd0_post_checksum_loop:
  add al, [si]
  inc si
  dec cl
  jnz hd0_post_checksum_loop
  not al ;; now take 2s complement
  inc al
  mov [si], al
;;; Done filling EBDA table for hard disk 0.
check_for_hd1:
  ;; is there really a second hard disk? if not, return now
  mov al, #0x12
  out #0x70, al
  in al, #0x71
  and al, #0x0f
  jnz post_d1_exists
  ret
post_d1_exists:
  ;; check that the hd type is really 0x0f.
  cmp al, #0x0f
  jz post_d1_extended
  HALT(9177)
post_d1_extended:
  ;; check that the extended type is 47 - user definable
  mov al, #0x1a
  out #0x70, al
  in al, #0x71
  cmp al, #47 ;; decimal 47 - user definable
  je post_d1_type47
  HALT(9185)
post_d1_type47:
  ;; Table for disk1.
  ;; CMOS purpose param table offset
  ;; 0x24 cylinders low 0
  ;; 0x25 cylinders high 1
  ;; 0x26 heads 2
  ;; 0x27 write pre-comp low 5
  ;; 0x28 write pre-comp high 6
  ;; 0x29 heads>8 8
  ;; 0x2a landing zone low C
  ;; 0x2b landing zone high D
  ;; 0x2c sectors/track E
;;; Fill EBDA table for hard disk 1.
  xor ax, ax
  mov ds, ax
  mov ax, word ptr [0x40E] ;; EBDA segment
  mov ds, ax
  mov al, #0x28
  out #0x70, al
  in al, #0x71
  mov ah, al
  mov al, #0x27
  out #0x70, al
  in al, #0x71
  mov (0x004d + 0x05), ax ;; write precomp word
  mov al, #0x29
  out #0x70, al
  in al, #0x71
  mov (0x004d + 0x08), al ;; drive control byte
  mov al, #0x2b
  out #0x70, al
  in al, #0x71
  mov ah, al
  mov al, #0x2a
  out #0x70, al
  in al, #0x71
  mov (0x004d + 0x0C), ax ;; landing zone word
  mov al, #0x25 ;; get cylinders word in AX
  out #0x70, al
  in al, #0x71 ;; high byte
  mov ah, al
  mov al, #0x24
  out #0x70, al
  in al, #0x71 ;; low byte
  mov bx, ax ;; BX = cylinders
  mov al, #0x26
  out #0x70, al
  in al, #0x71
  mov cl, al ;; CL = heads
  mov al, #0x2c
  out #0x70, al
  in al, #0x71
  mov dl, al ;; DL = sectors
  cmp bx, #1024
  jnbe hd1_post_logical_chs ;; if cylinders > 1024, use translated style CHS
hd1_post_physical_chs:
  ;; no logical CHS mapping used, just physical CHS
  ;; use Standard Fixed Disk Parameter Table (FDPT)
  mov (0x004d + 0x00), bx ;; number of physical cylinders
  mov (0x004d + 0x02), cl ;; number of physical heads
  mov (0x004d + 0x0E), dl ;; number of physical sectors
  ret
hd1_post_logical_chs:
  ;; complies with Phoenix style Translated Fixed Disk Parameter Table (FDPT)
  mov (0x004d + 0x09), bx ;; number of physical cylinders
  mov (0x004d + 0x0b), cl ;; number of physical heads
  mov (0x004d + 0x04), dl ;; number of physical sectors
  mov (0x004d + 0x0e), dl ;; number of logical sectors (same)
  mov al, #0xa0
  mov (0x004d + 0x03), al ;; A0h signature, indicates translated table
  cmp bx, #2048
  jnbe hd1_post_above_2048
  ;; 1024 < c <= 2048 cylinders
  shr bx, #0x01
  shl cl, #0x01
  jmp hd1_post_store_logical
hd1_post_above_2048:
  cmp bx, #4096
  jnbe hd1_post_above_4096
  ;; 2048 < c <= 4096 cylinders
  shr bx, #0x02
  shl cl, #0x02
  jmp hd1_post_store_logical
hd1_post_above_4096:
  cmp bx, #8192
  jnbe hd1_post_above_8192
  ;; 4096 < c <= 8192 cylinders
  shr bx, #0x03
  shl cl, #0x03
  jmp hd1_post_store_logical
hd1_post_above_8192:
  ;; 8192 < c <= 16384 cylinders
  shr bx, #0x04
  shl cl, #0x04
hd1_post_store_logical:
  mov (0x004d + 0x00), bx ;; number of physical cylinders
  mov (0x004d + 0x02), cl ;; number of physical heads
  ;; checksum
  mov cl, #0x0f ;; repeat count
  mov si, #0x004d ;; offset to disk0 FDPT
  mov al, #0x00 ;; sum
hd1_post_checksum_loop:
  add al, [si]
  inc si
  dec cl
  jnz hd1_post_checksum_loop
  not al ;; now take 2s complement
  inc al
  mov [si], al
;;; Done filling EBDA table for hard disk 1.
  ret
;--------------------
;- POST: EBDA segment
;--------------------
; relocated here because the primary POST area isnt big enough.
ebda_post:
  mov ax, #0x9FC0
  mov ds, ax
  mov byte ptr [0x0], #1
  xor ax, ax ; mov EBDA seg into 40E
  mov ds, ax
  mov word ptr [0x40E], #0x9FC0
  ret;;
;--------------------
;- POST: EOI + jmp via [0x40:67)
;--------------------
; relocated here because the primary POST area isnt big enough.
eoi_jmp_post:
  mov al, #0x20
  out #0xA0, al ;; slave PIC EOI
  mov al, #0x20
  out #0x20, al ;; master PIC EOI
jmp_post_0x467:
  xor ax, ax
  mov ds, ax
  jmp far ptr [0x467]
iret_post_0x467:
  xor ax, ax
  mov ds, ax
  mov sp, [0x467]
  mov ss, [0x469]
  iret
retf_post_0x467:
  xor ax, ax
  mov ds, ax
  mov sp, [0x467]
  mov ss, [0x469]
  retf
s3_post:
  call _s3_resume
  mov bl, #0x00
  and ax, ax
  jz normal_post
  call _s3_resume_panic
;--------------------
eoi_both_pics:
  mov al, #0x20
  out #0xA0, al ;; slave PIC EOI
eoi_master_pic:
  mov al, #0x20
  out #0x20, al ;; master PIC EOI
  ret
;--------------------
BcdToBin:
  ;; in: AL in BCD format
  ;; out: AL in binary format, AH will always be 0
  ;; trashes BX
  mov bl, al
  and bl, #0x0f ;; bl has low digit
  shr al, #4 ;; al has high digit
  mov bh, #10
  mul al, bh ;; multiply high digit by 10 (result in AX)
  add al, bl ;; then add low digit
  ret
;--------------------
timer_tick_post:
  ;; Setup the Timer Ticks Count (0x46C:dword) and
  ;; Timer Ticks Roller Flag (0x470:byte)
  ;; The Timer Ticks Count needs to be set according to
  ;; the current CMOS time, as if ticks have been occurring
  ;; at 18.2hz since midnight up to this point. Calculating
  ;; this is a little complicated. Here are the factors I gather
  ;; regarding this. 14,318,180 hz was the original clock speed,
  ;; chosen so it could be divided by either 3 to drive the 5Mhz CPU
  ;; at the time, or 4 to drive the CGA video adapter. The div3
  ;; source was divided again by 4 to feed a 1.193Mhz signal to
  ;; the timer. With a maximum 16bit timer count, this is again
  ;; divided down by 65536 to 18.2hz.
  ;;
  ;; 14,318,180 Hz clock
  ;; /3 = 4,772,726 Hz fed to orginal 5Mhz CPU
  ;; /4 = 1,193,181 Hz fed to timer
  ;; /65536 (maximum timer count) = 18.20650736 ticks/second
  ;; 1 second = 18.20650736 ticks
  ;; 1 minute = 1092.390442 ticks
  ;; 1 hour = 65543.42651 ticks
  ;;
  ;; Given the values in the CMOS clock, one could calculate
  ;; the number of ticks by the following:
  ;; ticks = (BcdToBin(seconds) * 18.206507) +
  ;; (BcdToBin(minutes) * 1092.3904)
  ;; (BcdToBin(hours) * 65543.427)
  ;; To get a little more accuracy, since Im using integer
  ;; arithmatic, I use:
  ;; ticks = (BcdToBin(seconds) * 18206507) / 1000000 +
  ;; (BcdToBin(minutes) * 10923904) / 10000 +
  ;; (BcdToBin(hours) * 65543427) / 1000
  ;; assuming DS=0000
  ;; get CMOS seconds
  xor eax, eax ;; clear EAX
  mov al, #0x00
  out #0x70, al
  in al, #0x71 ;; AL has CMOS seconds in BCD
  call BcdToBin ;; EAX now has seconds in binary
  mov edx, #18206507
  mul eax, edx
  mov ebx, #1000000
  xor edx, edx
  div eax, ebx
  mov ecx, eax ;; ECX will accumulate total ticks
  ;; get CMOS minutes
  xor eax, eax ;; clear EAX
  mov al, #0x02
  out #0x70, al
  in al, #0x71 ;; AL has CMOS minutes in BCD
  call BcdToBin ;; EAX now has minutes in binary
  mov edx, #10923904
  mul eax, edx
  mov ebx, #10000
  xor edx, edx
  div eax, ebx
  add ecx, eax ;; add to total ticks
  ;; get CMOS hours
  xor eax, eax ;; clear EAX
  mov al, #0x04
  out #0x70, al
  in al, #0x71 ;; AL has CMOS hours in BCD
  call BcdToBin ;; EAX now has hours in binary
  mov edx, #65543427
  mul eax, edx
  mov ebx, #1000
  xor edx, edx
  div eax, ebx
  add ecx, eax ;; add to total ticks
  mov 0x46C, ecx ;; Timer Ticks Count
  xor al, al
  mov 0x470, al ;; Timer Ticks Rollover Flag
  ret
;--------------------
int76_handler:
  ;; record completion in BIOS task complete flag
  push ax
  push ds
  mov ax, #0x0040
  mov ds, ax
  mov 0x008E, #0xff
  call eoi_both_pics
  pop ds
  pop ax
  iret
;--------------------
use32 386
apm32_out_str:
  push eax
  push ebx
  mov ebx, eax
apm32_out_str1:
  SEG CS
  mov al, byte ptr [bx]
  cmp al, #0
  je apm32_out_str2
  outb dx, al
  inc ebx
  jmp apm32_out_str1
apm32_out_str2:
  pop ebx
  pop eax
  ret
apm32_07_poweroff_str:
  .ascii "Shutdown"
  db 0
apm32_07_suspend_str:
  .ascii "Suspend"
  db 0
apm32_07_standby_str:
  .ascii "Standby"
  db 0
_apm32_entry:
  pushf
;-----------------
; APM interface disconnect
apm32_04:
  cmp al, #0x04
  jne apm32_05
  jmp apm32_ok
;-----------------
; APM cpu idle
apm32_05:
  cmp al, #0x05
  jne apm32_07
  pushf ; XEN
  sti ; XEN: OS calls us with ints disabled -- better re-enable here!
  hlt
  popf ; XEN
  jmp apm32_ok
;-----------------
; APM Set Power State
apm32_07:
  cmp al, #0x07
  jne apm32_08
  cmp bx, #1
  jne apm32_ok
  cmp cx, #3
  je apm32_07_poweroff
  cmp cx, #2
  je apm32_07_suspend
  cmp cx, #1
  je apm32_07_standby
  jne apm32_ok
apm32_07_poweroff:
  cli
  mov dx, #0x8900
  mov ax, #apm32_07_poweroff_str
  call apm32_out_str
apm32_07_1:
  hlt
  jmp apm32_07_1
apm32_07_suspend:
  push edx
  mov dx, #0x8900
  mov ax, #apm32_07_suspend_str
  call apm32_out_str
  pop edx
  jmp apm32_ok
apm32_07_standby:
  push edx
  mov dx, #0x8900
  mov ax, #apm32_07_standby_str
  call apm32_out_str
  pop edx
  jmp apm32_ok
;-----------------
; APM Enable / Disable
apm32_08:
  cmp al, #0x08
  jne apm32_0a
  jmp apm32_ok
;-----------------
; Get Power Status
apm32_0a:
  cmp al, #0x0a
  jne apm32_0b
  mov bh, #0x01
  mov bl, #0xff
  mov ch, #0x80
  mov cl, #0xff
  mov dx, #0xffff
  mov si, #0
  jmp apm32_ok
;-----------------
; Get PM Event
apm32_0b:
  cmp al, #0x0b
  jne apm32_0e
  mov ah, #0x80
  jmp apm32_error
;-----------------
; APM Driver Version
apm32_0e:
  cmp al, #0x0e
  jne apm32_0f
  mov ah, #1
  mov al, #2
  jmp apm32_ok
;-----------------
; APM Engage / Disengage
apm32_0f:
  cmp al, #0x0f
  jne apm32_10
  jmp apm32_ok
;-----------------
; APM Get Capabilities
apm32_10:
  cmp al, #0x10
  jne apm32_unimplemented
  mov bl, #0
  mov cx, #0
  jmp apm32_ok
;-----------------
apm32_ok:
  popf
  clc
  retf
apm32_unimplemented:
apm32_error:
  popf
  stc
  retf
use16 386
apm16_out_str:
  push eax
  push ebx
  mov ebx, eax
apm16_out_str1:
  SEG CS
  mov al, byte ptr [bx]
  cmp al, #0
  je apm16_out_str2
  outb dx, al
  inc ebx
  jmp apm16_out_str1
apm16_out_str2:
  pop ebx
  pop eax
  ret
apm16_07_poweroff_str:
  .ascii "Shutdown"
  db 0
apm16_07_suspend_str:
  .ascii "Suspend"
  db 0
apm16_07_standby_str:
  .ascii "Standby"
  db 0
_apm16_entry:
  pushf
;-----------------
; APM interface disconnect
apm16_04:
  cmp al, #0x04
  jne apm16_05
  jmp apm16_ok
;-----------------
; APM cpu idle
apm16_05:
  cmp al, #0x05
  jne apm16_07
  pushf ; XEN
  sti ; XEN: OS calls us with ints disabled -- better re-enable here!
  hlt
  popf ; XEN
  jmp apm16_ok
;-----------------
; APM Set Power State
apm16_07:
  cmp al, #0x07
  jne apm16_08
  cmp bx, #1
  jne apm16_ok
  cmp cx, #3
  je apm16_07_poweroff
  cmp cx, #2
  je apm16_07_suspend
  cmp cx, #1
  je apm16_07_standby
  jne apm16_ok
apm16_07_poweroff:
  cli
  mov dx, #0x8900
  mov ax, #apm16_07_poweroff_str
  call apm16_out_str
apm16_07_1:
  hlt
  jmp apm16_07_1
apm16_07_suspend:
  push edx
  mov dx, #0x8900
  mov ax, #apm16_07_suspend_str
  call apm16_out_str
  pop edx
  jmp apm16_ok
apm16_07_standby:
  push edx
  mov dx, #0x8900
  mov ax, #apm16_07_standby_str
  call apm16_out_str
  pop edx
  jmp apm16_ok
;-----------------
; APM Enable / Disable
apm16_08:
  cmp al, #0x08
  jne apm16_0a
  jmp apm16_ok
;-----------------
; Get Power Status
apm16_0a:
  cmp al, #0x0a
  jne apm16_0b
  mov bh, #0x01
  mov bl, #0xff
  mov ch, #0x80
  mov cl, #0xff
  mov dx, #0xffff
  mov si, #0
  jmp apm16_ok
;-----------------
; Get PM Event
apm16_0b:
  cmp al, #0x0b
  jne apm16_0e
  mov ah, #0x80
  jmp apm16_error
;-----------------
; APM Driver Version
apm16_0e:
  cmp al, #0x0e
  jne apm16_0f
  mov ah, #1
  mov al, #2
  jmp apm16_ok
;-----------------
; APM Engage / Disengage
apm16_0f:
  cmp al, #0x0f
  jne apm16_10
  jmp apm16_ok
;-----------------
; APM Get Capabilities
apm16_10:
  cmp al, #0x10
  jne apm16_unimplemented
  mov bl, #0
  mov cx, #0
  jmp apm16_ok
;-----------------
apm16_ok:
  popf
  clc
  retf
apm16_unimplemented:
apm16_error:
  popf
  stc
  retf
apmreal_out_str:
  push eax
  push ebx
  mov ebx, eax
apmreal_out_str1:
  SEG CS
  mov al, byte ptr [bx]
  cmp al, #0
  je apmreal_out_str2
  outb dx, al
  inc ebx
  jmp apmreal_out_str1
apmreal_out_str2:
  pop ebx
  pop eax
  ret
apmreal_07_poweroff_str:
  .ascii "Shutdown"
  db 0
apmreal_07_suspend_str:
  .ascii "Suspend"
  db 0
apmreal_07_standby_str:
  .ascii "Standby"
  db 0
  pushf
_apmreal_entry:
;-----------------
; APM installation check
apmreal_00:
  cmp al, #0x00
  jne apmreal_01
  mov ah, #1
  mov al, #2
  mov bh, #0x50
  mov bl, #0x4d
  mov cx, #0x3
  jmp apmreal_ok
;-----------------
; APM real mode interface connect
apmreal_01:
  cmp al, #0x01
  jne apmreal_02
  jmp apmreal_ok
;-----------------
; APM 16 bit protected mode interface connect
apmreal_02:
  cmp al, #0x02
  jne apmreal_03
  mov bx, #_apm16_entry
  mov ax, #0xf000
  mov si, #0xfff0
  mov cx, #0xf000
  mov di, #0xfff0
  jmp apmreal_ok
;-----------------
; APM 32 bit protected mode interface connect
apmreal_03:
  cmp al, #0x03
  jne apmreal_04
  mov ax, #0xf000
  mov ebx, #_apm32_entry
  mov cx, #0xf000
  mov esi, #0xfff0fff0
  mov dx, #0xf000
  mov di, #0xfff0
  jmp apmreal_ok
;-----------------
; APM interface disconnect
apmreal_04:
  cmp al, #0x04
  jne apmreal_05
  jmp apmreal_ok
;-----------------
; APM cpu idle
apmreal_05:
  cmp al, #0x05
  jne apmreal_07
  pushf ; XEN
  sti ; XEN: OS calls us with ints disabled -- better re-enable here!
  hlt
  popf ; XEN
  jmp apmreal_ok
;-----------------
; APM Set Power State
apmreal_07:
  cmp al, #0x07
  jne apmreal_08
  cmp bx, #1
  jne apmreal_ok
  cmp cx, #3
  je apmreal_07_poweroff
  cmp cx, #2
  je apmreal_07_suspend
  cmp cx, #1
  je apmreal_07_standby
  jne apmreal_ok
apmreal_07_poweroff:
  cli
  mov dx, #0x8900
  mov ax, #apmreal_07_poweroff_str
  call apmreal_out_str
apmreal_07_1:
  hlt
  jmp apmreal_07_1
apmreal_07_suspend:
  push edx
  mov dx, #0x8900
  mov ax, #apmreal_07_suspend_str
  call apmreal_out_str
  pop edx
  jmp apmreal_ok
apmreal_07_standby:
  push edx
  mov dx, #0x8900
  mov ax, #apmreal_07_standby_str
  call apmreal_out_str
  pop edx
  jmp apmreal_ok
;-----------------
; APM Enable / Disable
apmreal_08:
  cmp al, #0x08
  jne apmreal_0a
  jmp apmreal_ok
;-----------------
; Get Power Status
apmreal_0a:
  cmp al, #0x0a
  jne apmreal_0b
  mov bh, #0x01
  mov bl, #0xff
  mov ch, #0x80
  mov cl, #0xff
  mov dx, #0xffff
  mov si, #0
  jmp apmreal_ok
;-----------------
; Get PM Event
apmreal_0b:
  cmp al, #0x0b
  jne apmreal_0e
  mov ah, #0x80
  jmp apmreal_error
;-----------------
; APM Driver Version
apmreal_0e:
  cmp al, #0x0e
  jne apmreal_0f
  mov ah, #1
  mov al, #2
  jmp apmreal_ok
;-----------------
; APM Engage / Disengage
apmreal_0f:
  cmp al, #0x0f
  jne apmreal_10
  jmp apmreal_ok
;-----------------
; APM Get Capabilities
apmreal_10:
  cmp al, #0x10
  jne apmreal_unimplemented
  mov bl, #0
  mov cx, #0
  jmp apmreal_ok
;-----------------
apmreal_ok:
  popf
  clc
  jmp iret_modify_cf
apmreal_unimplemented:
apmreal_error:
  popf
  stc
  jmp iret_modify_cf
    .align 16
gdt_base:
    .word 0,0
    .byte 0,0,0,0
gdt_entry_pm_32bit_cs:
    .word 0xffff, 0x0000
    .byte 0x00, 0x9b, 0xcf, 0x00
gdt_entry_pm_16bit_cs:
    .word 0xffff, 0x0000
    .byte 0xf0000 >> 16, 0x9b, 0x8f, 0x0
gdt_entry_pm_32bit_ds:
    .word 0xffff, 0x0000
    .byte 0x0, 0x93, 0xcf, 0x0
gdt_entry_pm_16bit_ds:
    .word 0xffff, 0x0000
    .byte 0x0, 0x93, 0x8f, 0x0
gdt_entry_end:
protmode_gdtdesc:
    .word (gdt_entry_end - gdt_base) - 1
    .long gdt_base | 0xf0000
realmode_gdtdesc:
    .word 0xffff
    .long 0x0
Upcall:
    ; Do an upcall into 32 bit space
    ;
    ; Input:
    ; bx: index of function to call
    ; Ouput:
    ; dx, ax: 32 bit result of call (even if 'void' is expected)
    ; Save caller state, stack frame offsets listed below
    pushf
    cli
    push ds
    push es
    push ss
    push esp
    ; Calculate protected-mode esp from ss:sp
    and esp, #0xffff
    xor eax, eax
    mov ax, ss
    shl eax, #4
    add esp, eax
    ; Switch to protected mode
    seg cs
    lgdt protmode_gdtdesc
    mov eax, cr0
    or al, #0x1 ; protected mode on
    mov cr0, eax
    jmpf DWORD (0xf0000|upcall1), #(gdt_entry_pm_32bit_cs - gdt_base)
upcall1:
    USE32
    mov ax, #(gdt_entry_pm_32bit_ds - gdt_base)
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; Marshal arguments and call 32-bit function
    mov ecx, #32/4
upcall2:
    push 32 -4+14[esp]
    loop upcall2
    mov eax, [0x000EA000 + 20]
    call eax
    add esp, #32
    mov ecx, eax ; Result in ecx
    ; Restore real-mode stack pointer
    xor eax, eax
    mov ax, 4[esp]
    mov bx, ax ; Real-mode ss in bx
    shl eax, 4
    sub esp, eax
    ; Return to real mode
    jmpf upcall3, #(gdt_entry_pm_16bit_cs - gdt_base)
upcall3:
    USE16
    mov ax, #(gdt_entry_pm_16bit_ds - gdt_base)
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov eax, cr0
    and al, #0xfe ; protected mode off
    mov cr0, eax
    jmpf upcall4, #0xf0000>>4
upcall4:
    seg cs
    lgdt realmode_gdtdesc
    ; Restore real-mode ss
    mov ss, bx
    ; Convert result into dx:ax format
    mov eax, ecx
    ror eax, #16
    mov dx, ax
    ror eax, #16
    ; Restore caller state and return
    pop esp
    pop bx ; skip ss
    pop es
    pop ds
    popf
    ret
MACRO DoUpcall
    mov bx, #?1
    jmp Upcall
MEND
_TCGInterruptHandler: DoUpcall(0)
_tcpa_acpi_init: DoUpcall(1)
_tcpa_extend_acpi_log: DoUpcall(2)
_tcpa_calling_int19h: DoUpcall(3)
_tcpa_returned_int19h: DoUpcall(4)
_tcpa_add_event_separators: DoUpcall(5)
_tcpa_wake_event: DoUpcall(6)
_tcpa_add_bootdevice: DoUpcall(7)
_tcpa_start_option_rom_scan: DoUpcall(8)
_tcpa_option_rom: DoUpcall(9)
_tcpa_ipl: DoUpcall(10)
_tcpa_measure_post: DoUpcall(11)
_tcpa_initialize_tpm: DoUpcall(12)
_get_s3_waking_vector: DoUpcall(13)
_pmm: DoUpcall(14)
 #endasm
 #asm
MACRO POST_MEASURE
 push word #0x000f
 push #?2
 push word #0x000f
 push #?1
 call _tcpa_measure_post
 add sp, #8
MEND
 #endasm
void
tcpa_do_measure_POSTs()
{
 #asm
 POST_MEASURE(post, nmi)
 POST_MEASURE(floppy_drive_post, hard_drive_post)
 POST_MEASURE(hard_drive_post, ebda_post)
 POST_MEASURE(ebda_post, eoi_jmp_post)
 POST_MEASURE(eoi_jmp_post, timer_tick_post)
 POST_MEASURE(timer_tick_post, int76_handler)
 ret
 #endasm
}
  void
int1a_function32(regs, ES, DS, FLAGS)
  pushad_regs_t regs;
  Bit16u ES, DS, FLAGS;
{
 Bit16u rc;
 ;
 switch (regs.u.r8.ah) {
 case 0xbb:
  if (regs.u.r8.al != 0 &&
      regs.u.r32.ebx != 0x41504354L) {
      FLAGS |= 0x0001;
      return;
  }
  switch(regs.u.r8.al) {
  case 0x00:
  case 0x01:
  case 0x02:
  case 0x03:
  case 0x04:
  case 0x05:
  case 0x06:
  case 0x07:
   TCGInterruptHandler(((Bit32u)get_SS() << 4) + (Bit32u)&regs,
                       ES, DS,
                       ((Bit32u)get_SS() << 4) + (Bit32u)&FLAGS);
   break;
  default:
   FLAGS |= 0x0001;
  }
  break;
 default:
  FLAGS |= 0x0001;
  break;
 }
 ;
}
 #asm
;--------------------
use32 386
.align 16
bios32_structure:
  db 0x5f, 0x33, 0x32, 0x5f ;; "_32_" signature
  dw bios32_entry_point, 0xf ;; 32 bit physical address
  db 0 ;; revision level
  ;; length in paragraphs and checksum stored in a word to prevent errors
  dw (~(((bios32_entry_point >> 8) + (bios32_entry_point & 0xff) + 0x32) & 0xff) << 8) + 0x01
  db 0,0,0,0,0 ;; reserved
.align 16
bios32_entry_point:
  pushfd
  cmp eax, #0x49435024 ;; "$PCI"
  jne unknown_service
  mov eax, #0x80000000
  mov dx, #0x0cf8
  out dx, eax
  mov dx, #0x0cfc
  in eax, dx
  cmp eax, #0x12378086 ;; i440FX PCI bridge
  jne unknown_service
  mov ebx, #0x000f0000
  mov ecx, #0
  mov edx, #pcibios_protected
  xor al, al
  jmp bios32_end
unknown_service:
  mov al, #0x80
bios32_end:
  popfd
  retf
.align 16
pcibios_protected:
  pushfd
  cli
  push esi
  push edi
  cmp al, #0x01 ;; installation check
  jne pci_pro_f02
  mov bx, #0x0210
  mov cx, #0
  mov edx, #0x20494350 ;; "PCI "
  mov al, #0x01
  jmp pci_pro_ok
pci_pro_f02: ;; find pci device
  cmp al, #0x02
  jne pci_pro_f03
  shl ecx, #16
  mov cx, dx
  xor bx, bx
  mov di, #0x00
pci_pro_devloop:
  call pci_pro_select_reg
  mov dx, #0x0cfc
  in eax, dx
  cmp eax, ecx
  jne pci_pro_nextdev
  cmp si, #0
  je pci_pro_ok
  dec si
pci_pro_nextdev:
  inc bx
  cmp bx, #0x0100
  jne pci_pro_devloop
  mov ah, #0x86
  jmp pci_pro_fail
pci_pro_f03: ;; find class code
  cmp al, #0x03
  jne pci_pro_f08
  xor bx, bx
  mov di, #0x08
pci_pro_devloop2:
  call pci_pro_select_reg
  mov dx, #0x0cfc
  in eax, dx
  shr eax, #8
  cmp eax, ecx
  jne pci_pro_nextdev2
  cmp si, #0
  je pci_pro_ok
  dec si
pci_pro_nextdev2:
  inc bx
  cmp bx, #0x0100
  jne pci_pro_devloop2
  mov ah, #0x86
  jmp pci_pro_fail
pci_pro_f08: ;; read configuration byte
  cmp al, #0x08
  jne pci_pro_f09
  call pci_pro_select_reg
  push edx
  mov dx, di
  and dx, #0x03
  add dx, #0x0cfc
  in al, dx
  pop edx
  mov cl, al
  jmp pci_pro_ok
pci_pro_f09: ;; read configuration word
  cmp al, #0x09
  jne pci_pro_f0a
  call pci_pro_select_reg
  push edx
  mov dx, di
  and dx, #0x02
  add dx, #0x0cfc
  in ax, dx
  pop edx
  mov cx, ax
  jmp pci_pro_ok
pci_pro_f0a: ;; read configuration dword
  cmp al, #0x0a
  jne pci_pro_f0b
  call pci_pro_select_reg
  push edx
  mov dx, #0x0cfc
  in eax, dx
  pop edx
  mov ecx, eax
  jmp pci_pro_ok
pci_pro_f0b: ;; write configuration byte
  cmp al, #0x0b
  jne pci_pro_f0c
  call pci_pro_select_reg
  push edx
  mov dx, di
  and dx, #0x03
  add dx, #0x0cfc
  mov al, cl
  out dx, al
  pop edx
  jmp pci_pro_ok
pci_pro_f0c: ;; write configuration word
  cmp al, #0x0c
  jne pci_pro_f0d
  call pci_pro_select_reg
  push edx
  mov dx, di
  and dx, #0x02
  add dx, #0x0cfc
  mov ax, cx
  out dx, ax
  pop edx
  jmp pci_pro_ok
pci_pro_f0d: ;; write configuration dword
  cmp al, #0x0d
  jne pci_pro_unknown
  call pci_pro_select_reg
  push edx
  mov dx, #0x0cfc
  mov eax, ecx
  out dx, eax
  pop edx
  jmp pci_pro_ok
pci_pro_unknown:
  mov ah, #0x81
pci_pro_fail:
  pop edi
  pop esi
  popfd
  stc
  retf
pci_pro_ok:
  xor ah, ah
  pop edi
  pop esi
  popfd
  clc
  retf
pci_pro_select_reg:
  push edx
  mov eax, #0x800000
  mov ax, bx
  shl eax, #8
  and di, #0xff
  or ax, di
  and al, #0xfc
  mov dx, #0x0cf8
  out dx, eax
  pop edx
  ret
use16 386
pcibios_real:
  push eax
  push dx
  mov eax, #0x80000000
  mov dx, #0x0cf8
  out dx, eax
  mov dx, #0x0cfc
  in eax, dx
  cmp eax, #0x12378086 ;; i440FX PCI bridge
  je pci_present
  pop dx
  pop eax
  mov ah, #0xff
  stc
  ret
pci_present:
  pop dx
  pop eax
  cmp al, #0x01 ;; installation check
  jne pci_real_f02
  mov ax, #0x0001
  mov bx, #0x0210
  mov cx, #0
  mov edx, #0x20494350 ;; "PCI "
  mov edi, #0xf0000
  mov di, #pcibios_protected
  clc
  ret
pci_real_f02: ;; find pci device
  push esi
  push edi
  cmp al, #0x02
  jne pci_real_f03
  shl ecx, #16
  mov cx, dx
  xor bx, bx
  mov di, #0x00
pci_real_devloop:
  call pci_real_select_reg
  mov dx, #0x0cfc
  in eax, dx
  cmp eax, ecx
  jne pci_real_nextdev
  cmp si, #0
  je pci_real_ok
  dec si
pci_real_nextdev:
  inc bx
  cmp bx, #0x0100
  jne pci_real_devloop
  mov dx, cx
  shr ecx, #16
  mov ax, #0x8602
  jmp pci_real_fail
pci_real_f03: ;; find class code
  cmp al, #0x03
  jne pci_real_f08
  xor bx, bx
  mov di, #0x08
pci_real_devloop2:
  call pci_real_select_reg
  mov dx, #0x0cfc
  in eax, dx
  shr eax, #8
  cmp eax, ecx
  jne pci_real_nextdev2
  cmp si, #0
  je pci_real_ok
  dec si
pci_real_nextdev2:
  inc bx
  cmp bx, #0x0100
  jne pci_real_devloop2
  mov dx, cx
  shr ecx, #16
  mov ax, #0x8603
  jmp pci_real_fail
pci_real_f08: ;; read configuration byte
  cmp al, #0x08
  jne pci_real_f09
  call pci_real_select_reg
  push dx
  mov dx, di
  and dx, #0x03
  add dx, #0x0cfc
  in al, dx
  pop dx
  mov cl, al
  jmp pci_real_ok
pci_real_f09: ;; read configuration word
  cmp al, #0x09
  jne pci_real_f0a
  call pci_real_select_reg
  push dx
  mov dx, di
  and dx, #0x02
  add dx, #0x0cfc
  in ax, dx
  pop dx
  mov cx, ax
  jmp pci_real_ok
pci_real_f0a: ;; read configuration dword
  cmp al, #0x0a
  jne pci_real_f0b
  call pci_real_select_reg
  push dx
  mov dx, #0x0cfc
  in eax, dx
  pop dx
  mov ecx, eax
  jmp pci_real_ok
pci_real_f0b: ;; write configuration byte
  cmp al, #0x0b
  jne pci_real_f0c
  call pci_real_select_reg
  push dx
  mov dx, di
  and dx, #0x03
  add dx, #0x0cfc
  mov al, cl
  out dx, al
  pop dx
  jmp pci_real_ok
pci_real_f0c: ;; write configuration word
  cmp al, #0x0c
  jne pci_real_f0d
  call pci_real_select_reg
  push dx
  mov dx, di
  and dx, #0x02
  add dx, #0x0cfc
  mov ax, cx
  out dx, ax
  pop dx
  jmp pci_real_ok
pci_real_f0d: ;; write configuration dword
  cmp al, #0x0d
  jne pci_real_f0e
  call pci_real_select_reg
  push dx
  mov dx, #0x0cfc
  mov eax, ecx
  out dx, eax
  pop dx
  jmp pci_real_ok
pci_real_f0e: ;; get irq routing options
  cmp al, #0x0e
  jne pci_real_unknown
  SEG ES
  cmp word ptr [di], #pci_routing_table_structure_end - pci_routing_table_structure_start
  jb pci_real_too_small
  SEG ES
  mov word ptr [di], #pci_routing_table_structure_end - pci_routing_table_structure_start
  pushf
  push ds
  push es
  push cx
  push si
  push di
  cld
  mov si, #pci_routing_table_structure_start
  push cs
  pop ds
  SEG ES
  mov cx, [di+2]
  SEG ES
  mov es, [di+4]
  mov di, cx
  mov cx, #pci_routing_table_structure_end - pci_routing_table_structure_start
  rep
      movsb
  pop di
  pop si
  pop cx
  pop es
  pop ds
  popf
  mov bx, #(1 << 9) | (1 << 11) ;; irq 9 and 11 are used
  jmp pci_real_ok
pci_real_too_small:
  SEG ES
  mov word ptr [di], #pci_routing_table_structure_end - pci_routing_table_structure_start
  mov ah, #0x89
  jmp pci_real_fail
pci_real_unknown:
  mov ah, #0x81
pci_real_fail:
  pop edi
  pop esi
  stc
  ret
pci_real_ok:
  xor ah, ah
  pop edi
  pop esi
  clc
  ret
pci_real_select_reg:
  push dx
  mov eax, #0x800000
  mov ax, bx
  shl eax, #8
  and di, #0xff
  or ax, di
  and al, #0xfc
  mov dx, #0x0cf8
  out dx, eax
  pop dx
  ret
.align 16
pci_routing_table_structure:
  db 0x24, 0x50, 0x49, 0x52 ;; "$PIR" signature
  db 0, 1 ;; version
  dw 32 + (6 * 16) ;; table size
  db 0 ;; PCI interrupt router bus
  db 0x08 ;; PCI interrupt router DevFunc
  dw 0x0000 ;; PCI exclusive IRQs
  dw 0x8086 ;; compatible PCI interrupt router vendor ID
  dw 0x122e ;; compatible PCI interrupt router device ID
  dw 0,0 ;; Miniport data
  db 0,0,0,0,0,0,0,0,0,0,0 ;; reserved
  db 0x37 ;; checksum
pci_routing_table_structure_start:
  ;; first slot entry PCI-to-ISA (embedded)
  db 0 ;; pci bus number
  db 0x08 ;; pci device number (bit 7-3)
  db 0x61 ;; link value INTA#: pointer into PCI2ISA config space
  dw 0x0c20 ;; IRQ bitmap INTA#
  db 0x62 ;; link value INTB#
  dw 0x0c20 ;; IRQ bitmap INTB#
  db 0x63 ;; link value INTC#
  dw 0x0c20 ;; IRQ bitmap INTC#
  db 0x60 ;; link value INTD#
  dw 0x0c20 ;; IRQ bitmap INTD#
  db 0 ;; physical slot (0 = embedded)
  db 0 ;; reserved
  ;; second slot entry: 1st PCI slot
  db 0 ;; pci bus number
  db 0x10 ;; pci device number (bit 7-3)
  db 0x62 ;; link value INTA#
  dw 0x0c20 ;; IRQ bitmap INTA#
  db 0x63 ;; link value INTB#
  dw 0x0c20 ;; IRQ bitmap INTB#
  db 0x60 ;; link value INTC#
  dw 0x0c20 ;; IRQ bitmap INTC#
  db 0x61 ;; link value INTD#
  dw 0x0c20 ;; IRQ bitmap INTD#
  db 1 ;; physical slot (0 = embedded)
  db 0 ;; reserved
  ;; third slot entry: 2nd PCI slot
  db 0 ;; pci bus number
  db 0x18 ;; pci device number (bit 7-3)
  db 0x63 ;; link value INTA#
  dw 0x0c20 ;; IRQ bitmap INTA#
  db 0x60 ;; link value INTB#
  dw 0x0c20 ;; IRQ bitmap INTB#
  db 0x61 ;; link value INTC#
  dw 0x0c20 ;; IRQ bitmap INTC#
  db 0x62 ;; link value INTD#
  dw 0x0c20 ;; IRQ bitmap INTD#
  db 2 ;; physical slot (0 = embedded)
  db 0 ;; reserved
  ;; 4th slot entry: 3rd PCI slot
  db 0 ;; pci bus number
  db 0x20 ;; pci device number (bit 7-3)
  db 0x60 ;; link value INTA#
  dw 0x0c20 ;; IRQ bitmap INTA#
  db 0x61 ;; link value INTB#
  dw 0x0c20 ;; IRQ bitmap INTB#
  db 0x62 ;; link value INTC#
  dw 0x0c20 ;; IRQ bitmap INTC#
  db 0x63 ;; link value INTD#
  dw 0x0c20 ;; IRQ bitmap INTD#
  db 3 ;; physical slot (0 = embedded)
  db 0 ;; reserved
  ;; 5th slot entry: 4rd PCI slot
  db 0 ;; pci bus number
  db 0x28 ;; pci device number (bit 7-3)
  db 0x61 ;; link value INTA#
  dw 0x0c20 ;; IRQ bitmap INTA#
  db 0x62 ;; link value INTB#
  dw 0x0c20 ;; IRQ bitmap INTB#
  db 0x63 ;; link value INTC#
  dw 0x0c20 ;; IRQ bitmap INTC#
  db 0x60 ;; link value INTD#
  dw 0x0c20 ;; IRQ bitmap INTD#
  db 4 ;; physical slot (0 = embedded)
  db 0 ;; reserved
  ;; 6th slot entry: 5rd PCI slot
  db 0 ;; pci bus number
  db 0x30 ;; pci device number (bit 7-3)
  db 0x62 ;; link value INTA#
  dw 0x0c20 ;; IRQ bitmap INTA#
  db 0x63 ;; link value INTB#
  dw 0x0c20 ;; IRQ bitmap INTB#
  db 0x60 ;; link value INTC#
  dw 0x0c20 ;; IRQ bitmap INTC#
  db 0x61 ;; link value INTD#
  dw 0x0c20 ;; IRQ bitmap INTD#
  db 5 ;; physical slot (0 = embedded)
  db 0 ;; reserved
pci_routing_table_structure_end:
; according to POST Memory Manager Specification Version 1.01
.align 16
pmm_structure:
  db 0x24,0x50,0x4d,0x4d ;; "$PMM" signature
  db 0x01 ;; revision
  db 16 ;; length
  db (-((pmm_entry_point>>8)+pmm_entry_point+0x20f))&0xff;; checksum
  dw pmm_entry_point,0xf000 ;; far call entrypoint
  db 0,0,0,0,0 ;; reserved
pmm_entry_point:
  pushf
  pushad
; Calculate protected-mode address of PMM function args
  xor eax, eax
  mov ax, sp
  xor ebx, ebx
  mov bx, ss
  shl ebx, 4
  lea ebx, [eax+ebx+38] ;; ebx=(ss<<4)+sp+4(far call)+2(pushf)+32(pushad)
  push ebx
;
; Stack layout at this point:
;
; : +0x0 +0x2 +0x4 +0x6 +0x8 +0xa +0xc +0xe
; -----------------------------------------------------------------------
; sp : [&arg1 ][edi ][esi ][ebp ]
; sp+0x10: [esp ][ebx ][edx ][ecx ]
; sp+0x20: [eax ][flags ][ip ][cs ][arg1 ][arg2, ...
;
  call _pmm
  mov bx, sp
SEG SS
  mov [bx+0x20], ax
SEG SS
  mov [bx+0x18], dx
  pop ebx
  popad
  popf
  retf
; parallel port detection: base address in DX, index in BX, timeout in CL
detect_parport:
  push dx
  add dx, #2
  in al, dx
  and al, #0xdf ; clear input mode
  out dx, al
  pop dx
  mov al, #0xaa
  out dx, al
  in al, dx
  cmp al, #0xaa
  jne no_parport
  push bx
  shl bx, #1
  mov [bx+0x408], dx ; Parallel I/O address
  pop bx
  mov [bx+0x478], cl ; Parallel printer timeout
  inc bx
no_parport:
  ret
; serial port detection: base address in DX, index in BX, timeout in CL
detect_serial:
  push dx
  inc dx
  mov al, #0x02
  out dx, al
  in al, dx
  cmp al, #0x02
  jne no_serial
  inc dx
  in al, dx
  cmp al, #0x02
  jne no_serial
  dec dx
  xor al, al
  out dx, al
  pop dx
  push bx
  shl bx, #1
  mov [bx+0x400], dx ; Serial I/O address
  pop bx
  mov [bx+0x47c], cl ; Serial timeout
  inc bx
  ret
no_serial:
  pop dx
  ret
rom_checksum:
  pusha
  push ds
  xor ax, ax
  xor bx, bx
  xor cx, cx
  xor dx, dx
  mov ch, [2]
  shl cx, #1
  jnc checksum_loop
  jz checksum_loop
  xchg dx, cx
  dec cx
checksum_loop:
  add al, [bx]
  inc bx
  loop checksum_loop
  test dx, dx
  je checksum_out
  add al, [bx]
  mov cx, dx
  mov dx, ds
  add dh, #0x10
  mov ds, dx
  xor dx, dx
  xor bx, bx
  jmp checksum_loop
checksum_out:
  and al, #0xff
  pop ds
  popa
  ret
;; We need a copy of this string, but we are not actually a PnP BIOS,
;; so make sure it is *not* aligned, so OSes will not see it if they scan.
.align 16
  db 0
pnp_string:
  .ascii "$PnP"
rom_scan:
  ;; Scan for existence of valid expansion ROMS.
  ;; Video ROM: from 0xC0000..0xC7FFF in 2k increments
  ;; General ROM: from 0xC8000..0xE9FFF in 2k increments
  ;; System ROM: only 0xF0000
  ;;
  ;; Header:
  ;; Offset Value
  ;; 0 0x55
  ;; 1 0xAA
  ;; 2 ROM length in 512-byte blocks
  ;; 3 ROM initialization entry point (FAR CALL)
rom_scan_loop:
  push ax ;; Save AX
  mov ds, cx
  mov ax, #0x0004 ;; start with increment of 4 (512-byte) blocks = 2k
  cmp [0], #0xAA55 ;; look for signature
  jne rom_scan_increment
  call rom_checksum
  jnz rom_scan_increment
  mov al, [2] ;; change increment to ROM length in 512-byte blocks
  ;; We want our increment in 512-byte quantities, rounded to
  ;; the nearest 2k quantity, since we only scan at 2k intervals.
  test al, #0x03
  jz block_count_rounded
  and al, #0xfc ;; needs rounding up
  add al, #0x04
block_count_rounded:
  push ax ;; Save AX
  push di ;; Save DI
  ;; Push addr of ROM entry point
  push cx ;; Push seg
  push #0x0003 ;; Push offset
  ;; Get the BDF into ax before invoking the option ROM
  mov bl, [2]
  mov al, bl
  shr al, #7
  cmp al, #1
  jne fetch_bdf
  mov ax, ds ;; Increment the DS since rom size larger than an segment
  add ax, #0x1000
  mov ds, ax
fetch_bdf:
  shl bx, #9
  xor ax, ax
  mov al, [bx]
  ;; Point ES:DI at "$PnP", which tells the ROM that we are a PnP BIOS.
  ;; That should stop it grabbing INT 19h; we will use its BEV instead.
  mov bx, #0xf000
  mov es, bx
  lea di, pnp_string
  xor bx, bx ;; Restore DS back to 0000:
  mov ds, bx
  mov bp, sp ;; Call ROM init routine using seg:off on stack
  db 0xff ;; call_far ss:[bp+0]
  db 0x5e
  db 0
  cli ;; In case expansion ROM BIOS turns IF on
  add sp, #2 ;; Pop offset value
  pop cx ;; Pop seg value (restore CX)
  ;; Look at the ROM's PnP Expansion header.  Properly, we're supposed
  ;; to init all the ROMs and then go back and build an IPL table of
  ;; all the bootable devices, but we can get away with one pass.
  mov ds, cx ;; ROM base
  mov bx, 0x001a ;; 0x1A is the offset into ROM header that contains...
  mov ax, [bx] ;; the offset of PnP expansion header, where...
  cmp ax, #0x5024 ;; we look for signature "$PnP"
  jne no_bev
  mov ax, 2[bx]
  cmp ax, #0x506e
  jne no_bev
  mov ax, 0x16[bx] ;; 0x16 is the offset of Boot Connection Vector
  cmp ax, #0x0000
  je no_bcv
  ;; Option ROM has BCV. Run it now.
  push cx ;; Push seg
  push ax ;; Push offset
  ;; Point ES:DI at "$PnP", which tells the ROM that we are a PnP BIOS.
  mov bx, #0xf000
  mov es, bx
  lea di, pnp_string
  mov bp, sp ;; Call ROM BCV routine using seg:off on stack
  db 0xff ;; call_far ss:[bp+0]
  db 0x5e
  db 0
  cli ;; In case expansion ROM BIOS turns IF on
  add sp, #2 ;; Pop offset value
  pop cx ;; Pop seg value (restore CX)
  jmp no_bev
no_bcv:
  mov ax, 0x1a[bx] ;; 0x1A is also the offset into the expansion header of...
  cmp ax, #0x0000 ;; the Bootstrap Entry Vector, or zero if there is 0 .
  je no_bev
  ;; Found a device that thinks it can boot the system. Record its BEV and product name string.
  mov di, 0x10[bx] ;; Pointer to the product name string or zero if 0
  xor bx, bx
  mov ds, bx
  mov bx, word ptr [0x40E] ;; EBDA segment
  mov ds, bx ;; Go to the segment where the IPL table lives
  mov bx, 0x0380 ;; Read the number of entries so far
  cmp bx, #8
  je no_bev ;; Get out if the table is full
  shl bx, #0x4 ;; Turn count into offset (entries are 16 bytes)
  mov 0x0300 +0[bx], #0x80 ;; This entry is a BEV device
  mov 0x0300 +6[bx], cx ;; Build a far pointer from the segment...
  mov 0x0300 +4[bx], ax ;; and the offset
  cmp di, #0x0000
  je no_prod_str
  mov 0xA[bx], cx ;; Build a far pointer from the segment...
  mov 8[bx], di ;; and the offset
no_prod_str:
  shr bx, #0x4 ;; Turn the offset back into a count
  inc bx ;; We have one more entry now
  mov 0x0380, bx ;; Remember that.
no_bev:
  pop di ;; Restore DI
  pop ax ;; Restore AX
rom_scan_increment:
  shl ax, #5 ;; convert 512-bytes blocks to 16-byte increments
                ;; because the segment selector is shifted left 4 bits.
  add cx, ax
  pop ax ;; Restore AX
  cmp cx, ax
  jbe rom_scan_loop
  xor ax, ax ;; Restore DS back to 0000:
  mov ds, ax
  ret
; Copy the SMBIOS entry point from where hvmloader left it.
; The entry point must be somewhere in 0xf0000-0xfffff on a 16-byte boundary,
; but the tables themselves can be elsewhere.
smbios_init:
  push ax
  push cx
  push es
  push ds
  push di
  push si
  mov cx, #0x001f ; 0x1f bytes to copy
  mov ax, #0xf000
  mov es, ax ; destination segment is 0xf0000
  mov di, #smbios_entry_point ; destination offset
  mov ax, #(0x000EB000>>4)
  mov ds, ax
  mov si, #(0x000EB000&15)
  cld
  rep
    movsb
  pop si
  pop di
  pop ds
  pop es
  pop cx
  pop ax
  ret
post_init_pic:
  mov al, #0x11 ; send initialisation commands
  out 0x20, al
  out 0xa0, al
  mov al, #0x08
  out 0x21, al
  mov al, #0x70
  out 0xa1, al
  mov al, #0x04
  out 0x21, al
  mov al, #0x02
  out 0xa1, al
  mov al, #0x01
  out 0x21, al
  out 0xa1, al
  mov al, #0xb8
  out 0x21, AL ;master pic: unmask IRQ 0, 1, 2, 6
  mov al, #0x8f
  out 0xa1, AL ;slave pic: unmask IRQ 12, 13, 14
  ret
  .align 16
smbios_entry_point:
  db 0,0,0,0,0,0,0,0 ; 8 bytes
  db 0,0,0,0,0,0,0,0 ; 16 bytes
  db 0,0,0,0,0,0,0,0 ; 24 bytes
  db 0,0,0,0,0,0,0 ; 31 bytes
;; the following area can be used to write dynamically generated tables
  .align 16
bios_table_area_start:
  db 0x5F, 0x5F, 0x5F, 0x48, 0x56, 0x4D, 0x4D, 0x50 ;; ___HVMMP
  dd bios_table_area_end - bios_table_area_start
;--------
;- POST -
;--------
.org 0xe05b ; POST Entry Point
post:
  xor ax, ax
  ;; first reset the DMA controllers
  out 0x0d,al
  out 0xda,al
  ;; then initialize the DMA controllers
  mov al, #0xC0
  out 0xD6, al ; cascade mode of channel 4 enabled
  mov al, #0x00
  out 0xD4, al ; unmask channel 4
  ;; Examine CMOS shutdown status.
  mov AL, #0x0f
  out 0x70, AL
  in AL, 0x71
  ;; backup status
  mov bl, al
  ;; Reset CMOS shutdown status.
  mov AL, #0x0f
  out 0x70, AL ; select CMOS register Fh
  mov AL, #0x00
  out 0x71, AL ; set shutdown action to normal
  ;; Examine CMOS shutdown status.
  mov al, bl
  ;; 0x00, 0x09, 0x0D+ = normal startup
  cmp AL, #0x00
  jz normal_post
  cmp AL, #0x0d
  jae normal_post
  cmp AL, #0x09
  je normal_post
  ;; 0x05 = eoi + jmp via [0x40:0x67] jump
  cmp al, #0x05
  je eoi_jmp_post
  ;; 0x0A = jmp via [0x40:0x67] jump
  cmp al, #0x0a
  je jmp_post_0x467
  ;; 0x0B = iret via [0x40:0x67]
  cmp al, #0x0b
  je iret_post_0x467
  ;; 0x0C = retf via [0x40:0x67]
  cmp al, #0x0c
  je retf_post_0x467
  ;; Examine CMOS shutdown status.
  ;; 0x01,0x02,0x03,0x04,0x06,0x07,0x08 = Unimplemented shutdown status.
  push bx
  call _shutdown_status_panic
normal_post:
  ; case 0: normal startup
  cli
  mov ax, #0x0ffe
  mov sp, ax
  mov ax, #0x9e00
  mov ss, ax
  ;; Save shutdown status
  mov 0x04b0, bl
  cmp bl, #0xfe
  jz s3_post
  ;; zero out BIOS data area (40:00..40:ff)
  mov es, ax
  mov cx, #0x0080 ;; 128 words
  mov di, #0x0400
  cld
  rep
    stosw
  call _log_bios_start
  ;; set all interrupts to default handler
  xor bx, bx ;; offset index
  mov cx, #0x0100 ;; counter (256 interrupts)
  mov ax, #dummy_iret_handler
  mov dx, #0xF000
post_default_ints:
  mov [bx], ax
  add bx, #2
  mov [bx], dx
  add bx, #2
  loop post_default_ints
  ;; set vector 0x79 to zero
  ;; this is used by 'gardian angel' protection system
  SET_INT_VECTOR(0x79, #0, #0)
  ;; base memory in K 40:13 (word)
  mov ax, #(640 - 1)
  mov 0x0413, ax
  ;; Manufacturing Test 40:12
  ;; zerod out above
  ;; Warm Boot Flag 0040:0072
  ;; value of 1234h = skip memory checks
  ;; zerod out above
  ;; Printer Services vector
  SET_INT_VECTOR(0x17, #0xF000, #int17_handler)
  ;; Bootstrap failure vector
  SET_INT_VECTOR(0x18, #0xF000, #int18_handler)
  ;; Bootstrap Loader vector
  SET_INT_VECTOR(0x19, #0xF000, #int19_handler)
  ;; User Timer Tick vector
  SET_INT_VECTOR(0x1c, #0xF000, #int1c_handler)
  ;; Memory Size Check vector
  SET_INT_VECTOR(0x12, #0xF000, #int12_handler)
  ;; Equipment Configuration Check vector
  SET_INT_VECTOR(0x11, #0xF000, #int11_handler)
  ;; System Services
  SET_INT_VECTOR(0x15, #0xF000, #int15_handler)
  ;; EBDA setup
  call ebda_post
  ;; PIT setup
  SET_INT_VECTOR(0x08, #0xF000, #int08_handler)
  ;; int 1C already points at dummy_iret_handler (above)
  mov al, #0x34 ; timer0: binary count, 16bit count, mode 2
  out 0x43, al
  mov al, #0x0b ; #0xe90b = 20 Hz (temporary, until we fix xen/vmx support)
  out 0x40, al ; lsb
  mov al, #0xe9
  out 0x40, al ; msb
  ;; Keyboard
  SET_INT_VECTOR(0x09, #0xF000, #int09_handler)
  SET_INT_VECTOR(0x16, #0xF000, #int16_handler)
  xor ax, ax
  mov ds, ax
  mov 0x0417, al
  mov 0x0418, al
  mov 0x0419, al
  mov 0x0471, al
  mov 0x0497, al
  mov al, #0x10
  mov 0x0496, al
  mov bx, #0x001E
  mov 0x041A, bx
  mov 0x041C, bx
  mov bx, #0x001E
  mov 0x0480, bx
  mov bx, #0x003E
  mov 0x0482, bx
  call _keyboard_init
  ;; mov CMOS Equipment Byte to BDA Equipment Word
  mov ax, 0x0410
  mov al, #0x14
  out 0x70, al
  in al, 0x71
  mov 0x0410, ax
  ;; Parallel setup
  SET_INT_VECTOR(0x0F, #0xF000, #dummy_iret_handler)
  xor ax, ax
  mov ds, ax
  xor bx, bx
  mov cl, #0x14 ; timeout value
  mov dx, #0x378 ; Parallel I/O address, port 1
  call detect_parport
  mov dx, #0x278 ; Parallel I/O address, port 2
  call detect_parport
  shl bx, #0x0e
  mov ax, 0x410 ; Equipment word bits 14..15 determing # parallel ports
  and ax, #0x3fff
  or ax, bx ; set number of parallel ports
  mov 0x410, ax
  ;; Serial setup
  SET_INT_VECTOR(0x0C, #0xF000, #dummy_iret_handler)
  SET_INT_VECTOR(0x14, #0xF000, #int14_handler)
  xor bx, bx
  mov cl, #0x0a ; timeout value
  mov dx, #0x03f8 ; Serial I/O address, port 1
  call detect_serial
  mov dx, #0x02f8 ; Serial I/O address, port 2
  call detect_serial
  mov dx, #0x03e8 ; Serial I/O address, port 3
  call detect_serial
  mov dx, #0x02e8 ; Serial I/O address, port 4
  call detect_serial
  shl bx, #0x09
  mov ax, 0x410 ; Equipment word bits 9..11 determing # serial ports
  and ax, #0xf1ff
  or ax, bx ; set number of serial port
  mov 0x410, ax
  ;; CMOS RTC
  SET_INT_VECTOR(0x1A, #0xF000, #int1a_handler)
  SET_INT_VECTOR(0x4A, #0xF000, #dummy_iret_handler)
  SET_INT_VECTOR(0x70, #0xF000, #int70_handler)
  ;; BIOS DATA AREA 0x4CE ???
  call timer_tick_post
  ;; PS/2 mouse setup
  SET_INT_VECTOR(0x74, #0xF000, #int74_handler)
  ;; IRQ13 (FPU exception) setup
  SET_INT_VECTOR(0x75, #0xF000, #int75_handler)
  ;; Video setup
  SET_INT_VECTOR(0x10, #0xF000, #int10_handler)
  ;; PIC
  call post_init_pic
  mov cx, #0xc000 ;; init vga bios
  mov ax, #0xc780
  call rom_scan
  call _print_bios_banner
  ;;
  ;; Floppy setup
  ;;
  call floppy_drive_post
  ;;
  ;; Hard Drive setup
  ;;
  call hard_drive_post
  ;;
  ;; ATA/ATAPI driver setup
  ;;
  call _ata_init
  call _ata_detect
  ;;
  ;;
  ;; eltorito floppy/harddisk emulation from cd
  ;;
  call _cdemu_init
  ;;
  call _enable_rom_write_access
  call _clobber_entry_point
  call _fixup_base_mem_in_k
  call smbios_init
  call _init_boot_vectors
  mov cx, #(0x000C8000 >> 4) ;; init option roms
  mov ax, #(0x000EA000 >> 4)
  call rom_scan
  call _disable_rom_write_access
  call _interactive_bootkey
  sti ;; enable interrupts
  ;; Start the boot sequence. See the comments in int19_relocated
  ;; for why we use INT 18h instead of INT 19h here.
  int #0x18
.org 0xe2c3 ; NMI Handler Entry Point
nmi:
  ;; FIXME the NMI handler should not panic
  ;; but iret when called from int75 (fpu exception)
  call _nmi_handler_msg
  iret
int75_handler:
  out 0xf0, al
  call eoi_both_pics
  int 2
  iret
;-------------------------------------------
;- INT 13h Fixed Disk Services Entry Point -
;-------------------------------------------
.org 0xe3fe ; INT 13h Fixed Disk Services Entry Point
int13_handler:
  jmp int13_relocated
.org 0xe401 ; Fixed Disk Parameter Table
;----------
;- INT19h -
;----------
.org 0xe6f2 ; INT 19h Boot Load Service Entry Point
int19_handler:
  jmp int19_relocated
;-------------------------------------------
;- System BIOS Configuration Data Table
;-------------------------------------------
.org 0xe6f5
db 0x08 ; Table size (bytes) -Lo
db 0x00 ; Table size (bytes) -Hi
db 0xFC
db 0x00
db 1
; Feature byte 1
; b7: 1=DMA channel 3 used by hard disk
; b6: 1=2 interrupt controllers present
; b5: 1=RTC present
; b4: 1=BIOS calls int 15h/4Fh every key
; b3: 1=wait for extern event supported (Int 15h/41h)
; b2: 1=extended BIOS data area used
; b1: 0=AT or ESDI bus, 1=MicroChannel
; b0: 1=Dual bus (MicroChannel + ISA)
db (0 << 7) | (1 << 6) | (1 << 5) | (1 << 4) | (0 << 3) | (1 << 2) | (0 << 1) | (0 << 0)
; Feature byte 2
; b7: 1=32-bit DMA supported
; b6: 1=int16h, function 9 supported
; b5: 1=int15h/C6h (get POS data) supported
; b4: 1=int15h/C7h (get mem map info) supported
; b3: 1=int15h/C8h (en/dis CPU) supported
; b2: 1=non-8042 kb controller
; b1: 1=data streaming supported
; b0: reserved
db (0 << 7) | (1 << 6) | (0 << 5) | (0 << 4) | (0 << 3) | (0 << 2) | (0 << 1) | (0 << 0)
; Feature byte 3
; b7: not used
; b6: reserved
; b5: reserved
; b4: POST supports ROM-to-RAM enable/disable
; b3: SCSI on system board
; b2: info panel installed
; b1: Initial Machine Load (IML) system - BIOS on disk
; b0: SCSI supported in IML
db 0x00
; Feature byte 4
; b7: IBM private
; b6: EEPROM present
; b5-3: ABIOS presence (011 = not supported)
; b2: private
; b1: memory split above 16Mb supported
; b0: POSTEXT directly supported by POST
db 0x00
; Feature byte 5 (IBM)
; b1: enhanced mouse
; b0: flash EPROM
db 0x00
.org 0xe729 ; Baud Rate Generator Table
;----------
;- INT14h -
;----------
.org 0xe739 ; INT 14h Serial Communications Service Entry Point
int14_handler:
  push ds
  pusha
  xor ax, ax
  mov ds, ax
  call _int14_function
  popa
  pop ds
  iret
;----------------------------------------
;- INT 16h Keyboard Service Entry Point -
;----------------------------------------
.org 0xe82e
int16_handler:
  sti
  push ds
  pushf
  pusha
  cmp ah, #0x00
  je int16_F00
  cmp ah, #0x10
  je int16_F00
  mov bx, #0xf000
  mov ds, bx
  call _int16_function
  popa
  popf
  pop ds
  jz int16_zero_set
int16_zero_clear:
  push bp
  mov bp, sp
  and BYTE [bp + 0x06], #0xbf
  pop bp
  iret
int16_zero_set:
  push bp
  mov bp, sp
  or BYTE [bp + 0x06], #0x40
  pop bp
  iret
int16_F00:
  mov bx, #0x0040
  mov ds, bx
int16_wait_for_key:
  cli
  mov bx, 0x001a
  cmp bx, 0x001c
  jne int16_key_found
  sti
  hlt
  jmp int16_wait_for_key
int16_key_found:
  mov bx, #0xf000
  mov ds, bx
  call _int16_function
  popa
  popf
  pop ds
  iret
;-------------------------------------------------
;- INT09h : Keyboard Hardware Service Entry Point -
;-------------------------------------------------
.org 0xe987
int09_handler:
  cli
  push ax
  mov al, #0xAD ;;disable keyboard
  out #0x64, al
  mov al, #0x0B
  out #0x20, al
  in al, #0x20
  and al, #0x02
  jz int09_finish
  in al, #0x60 ;;read key from keyboard controller
  sti
  push ds
  pusha
  mov ah, #0x4f ;; allow for keyboard intercept
  stc
  int #0x15
  jnc int09_done
  ;; check for extended key
  cmp al, #0xe0
  jne int09_check_pause
  xor ax, ax
  mov ds, ax
  mov al, BYTE [0x496] ;; mf2_state |= 0x02
  or al, #0x02
  mov BYTE [0x496], al
  jmp int09_done
int09_check_pause: ;; check for pause key
  cmp al, #0xe1
  jne int09_process_key
  xor ax, ax
  mov ds, ax
  mov al, BYTE [0x496] ;; mf2_state |= 0x01
  or al, #0x01
  mov BYTE [0x496], al
  jmp int09_done
int09_process_key:
  mov bx, #0xf000
  mov ds, bx
  call _int09_function
int09_done:
  popa
  pop ds
  cli
  call eoi_master_pic
int09_finish:
  mov al, #0xAE ;;enable keyboard
  out #0x64, al
  pop ax
  iret
;----------------------------------------
;- INT 13h Diskette Service Entry Point -
;----------------------------------------
.org 0xec59
int13_diskette:
  jmp int13_noeltorito
;---------------------------------------------
;- INT 0Eh Diskette Hardware ISR Entry Point -
;---------------------------------------------
.org 0xef57 ; INT 0Eh Diskette Hardware ISR Entry Point
int0e_handler:
  push ax
  push dx
  mov dx, #0x03f4
  in al, dx
  and al, #0xc0
  cmp al, #0xc0
  je int0e_normal
  mov dx, #0x03f5
  mov al, #0x08 ; sense interrupt status
  out dx, al
int0e_loop1:
  mov dx, #0x03f4
  in al, dx
  and al, #0xc0
  cmp al, #0xc0
  jne int0e_loop1
int0e_loop2:
  mov dx, #0x03f5
  in al, dx
  mov dx, #0x03f4
  in al, dx
  and al, #0xc0
  cmp al, #0xc0
  je int0e_loop2
int0e_normal:
  push ds
  xor ax, ax ;; segment 0000
  mov ds, ax
  call eoi_master_pic
  mov al, 0x043e
  or al, #0x80 ;; diskette interrupt has occurred
  mov 0x043e, al
  pop ds
  pop dx
  pop ax
  iret
.org 0xefc7 ; Diskette Controller Parameter Table
diskette_param_table:
;; Since no provisions are made for multiple drive types, most
;; values in this table are ignored. I set parameters for 1.44M
;; floppy here
db 0xAF
db 0x02 ;; head load time 0000001, DMA used
db 0x25
db 0x02
db 18
db 0x1B
db 0xFF
db 0x6C
db 0xF6
db 0x0F
db 0x08
;----------------------------------------
;- INT17h : Printer Service Entry Point -
;----------------------------------------
.org 0xefd2
int17_handler:
  push ds
  pusha
  xor ax, ax
  mov ds, ax
  call _int17_function
  popa
  pop ds
  iret
diskette_param_table2:
;; New diskette parameter table adding 3 parameters from IBM
;; Since no provisions are made for multiple drive types, most
;; values in this table are ignored. I set parameters for 1.44M
;; floppy here
db 0xAF
db 0x02 ;; head load time 0000001, DMA used
db 0x25
db 0x02
db 18
db 0x1B
db 0xFF
db 0x6C
db 0xF6
db 0x0F
db 0x08
db 79 ;; maximum track
db 0 ;; data transfer rate
db 4 ;; drive type in cmos
.org 0xf045 ; INT 10 Functions 0-Fh Entry Point
  HALT(11468)
  iret
;----------
;- INT10h -
;----------
.org 0xf065 ; INT 10h Video Support Service Entry Point
int10_handler:
  ;; dont do anything, since the VGA BIOS handles int10h requests
  iret
.org 0xf0a4 ; MDA/CGA Video Parameter Table (INT 1Dh)
;----------
;- INT12h -
;----------
.org 0xf841 ; INT 12h Memory Size Service Entry Point
; ??? different for Pentium (machine check)?
int12_handler:
  push ds
  mov ax, #0x0040
  mov ds, ax
  mov ax, 0x0013
  pop ds
  iret
;----------
;- INT11h -
;----------
.org 0xf84d ; INT 11h Equipment List Service Entry Point
int11_handler:
  push ds
  mov ax, #0x0040
  mov ds, ax
  mov ax, 0x0010
  pop ds
  iret
;----------
;- INT15h -
;----------
.org 0xf859 ; INT 15h System Services Entry Point
int15_handler:
  pushf
  cmp ah, #0x53
  je apm_call
  push ds
  push es
  cmp ah, #0x86
  je int15_handler32
  cmp ah, #0xE8
  je int15_handler32
  pusha
  cmp ah, #0xC2
  je int15_handler_mouse
  call _int15_function
int15_handler_mouse_ret:
  popa
int15_handler32_ret:
  pop es
  pop ds
  popf
  jmp iret_modify_cf
apm_call:
  jmp _apmreal_entry
int15_handler_mouse:
  call _int15_function_mouse
  jmp int15_handler_mouse_ret
int15_handler32:
  pushad
  call _int15_function32
  popad
  jmp int15_handler32_ret
;; Protected mode IDT descriptor
;;
;; I just make the limit 0, so the machine will shutdown
;; if an exception occurs during protected mode memory
;; transfers.
;;
;; Set base to f0000 to correspond to beginning of BIOS,
;; in case I actually define an IDT later
;; Set limit to 0
pmode_IDT_info:
dw 0x0000 ;; limit 15:00
dw 0x0000 ;; base 15:00
db 0x0f ;; base 23:16
;; Real mode IDT descriptor
;;
;; Set to typical real-mode values.
;; base = 000000
;; limit = 03ff
rmode_IDT_info:
dw 0x03ff ;; limit 15:00
dw 0x0000 ;; base 15:00
db 0x00 ;; base 23:16
;----------
;- INT1Ah -
;----------
.org 0xfe6e ; INT 1Ah Time-of-day Service Entry Point
int1a_handler:
  cmp ah, #0xb1
  jne int1a_normal
  call pcibios_real
  jc pcibios_error
  retf 2
pcibios_error:
  mov bl, ah
  mov ah, #0xb1
  push ds
  pusha
  mov ax, ss ; set readable descriptor to ds, for calling pcibios
  mov ds, ax ; on 16bit protected mode.
  jmp int1a_callfunction
int1a_normal:
  push ds
  pusha
  xor ax, ax
  mov ds, ax
int1a_callfunction:
  call _int1a_function
  popa
  pop ds
  iret
;;
;; int70h: IRQ8 - CMOS RTC
;;
int70_handler:
  push ds
  pushad
  xor ax, ax
  mov ds, ax
  call _int70_function
  popad
  pop ds
  iret
;---------
;- INT08 -
;---------
.org 0xfea5 ; INT 08h System Timer ISR Entry Point
int08_handler:
  sti
  push eax
  push ds
  xor ax, ax
  mov ds, ax
  ;; time to turn off drive(s)?
  mov al,0x0440
  or al,al
  jz int08_floppy_off
  dec al
  mov 0x0440,al
  jnz int08_floppy_off
  ;; turn motor(s) off
  push dx
  mov dx,#0x03f2
  in al,dx
  and al,#0xcf
  out dx,al
  pop dx
int08_floppy_off:
  mov eax, 0x046c ;; get ticks dword
  inc eax
  ;; compare eax to one days worth of timer ticks at 18.2 hz
  cmp eax, #0x001800B0
  jb int08_store_ticks
  ;; there has been a midnight rollover at this point
  xor eax, eax ;; zero out counter
  inc BYTE 0x0470 ;; increment rollover flag
int08_store_ticks:
  mov 0x046c, eax ;; store new ticks dword
  ;; chain to user timer tick INT #0x1c
  int #0x1c
  cli
  call eoi_master_pic
  pop ds
  pop eax
  iret
.org 0xfef3 ; Initial Interrupt Vector Offsets Loaded by POST
.org 0xff00
.ascii "(c) 2002 MandrakeSoft S.A. Written by Kevin Lawton & the Bochs team."
;------------------------------------------------
;- IRET Instruction for Dummy Interrupt Handler -
;------------------------------------------------
.org 0xff53 ; IRET Instruction for Dummy Interrupt Handler
dummy_iret_handler:
  iret
.org 0xff54 ; INT 05h Print Screen Service Entry Point
  HALT(11702)
  iret
.org 0xfff0 ; Power-up Entry Point
   jmp 0xf000:post
.org 0xfff5 ; ASCII Date ROM was built - 8 characters in MM/DD/YY
.ascii "06/23/99"
.org 0xfffe ; System Model ID
db 0xFC
db 0x00 ; filler
.org 0xfa6e ;; Character Font for 320x200 & 640x200 Graphics (lower 128 characters)
 #endasm
static Bit8u vgafont8[128*8]=
{
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x7e, 0x81, 0xa5, 0x81, 0xbd, 0x99, 0x81, 0x7e,
 0x7e, 0xff, 0xdb, 0xff, 0xc3, 0xe7, 0xff, 0x7e,
 0x6c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 0x00,
 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 0x00,
 0x38, 0x7c, 0x38, 0xfe, 0xfe, 0x7c, 0x38, 0x7c,
 0x10, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x7c,
 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00,
 0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff,
 0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00,
 0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff,
 0x0f, 0x07, 0x0f, 0x7d, 0xcc, 0xcc, 0xcc, 0x78,
 0x3c, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x7e, 0x18,
 0x3f, 0x33, 0x3f, 0x30, 0x30, 0x70, 0xf0, 0xe0,
 0x7f, 0x63, 0x7f, 0x63, 0x63, 0x67, 0xe6, 0xc0,
 0x99, 0x5a, 0x3c, 0xe7, 0xe7, 0x3c, 0x5a, 0x99,
 0x80, 0xe0, 0xf8, 0xfe, 0xf8, 0xe0, 0x80, 0x00,
 0x02, 0x0e, 0x3e, 0xfe, 0x3e, 0x0e, 0x02, 0x00,
 0x18, 0x3c, 0x7e, 0x18, 0x18, 0x7e, 0x3c, 0x18,
 0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,
 0x7f, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x1b, 0x00,
 0x3e, 0x63, 0x38, 0x6c, 0x6c, 0x38, 0xcc, 0x78,
 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x7e, 0x00,
 0x18, 0x3c, 0x7e, 0x18, 0x7e, 0x3c, 0x18, 0xff,
 0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x00,
 0x18, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 0x00,
 0x00, 0x18, 0x0c, 0xfe, 0x0c, 0x18, 0x00, 0x00,
 0x00, 0x30, 0x60, 0xfe, 0x60, 0x30, 0x00, 0x00,
 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xfe, 0x00, 0x00,
 0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00,
 0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00,
 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x30, 0x78, 0x78, 0x30, 0x30, 0x00, 0x30, 0x00,
 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00,
 0x30, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x30, 0x00,
 0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00,
 0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00,
 0x60, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00,
 0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00,
 0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00,
 0x00, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60,
 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,
 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00,
 0x7c, 0xc6, 0xce, 0xde, 0xf6, 0xe6, 0x7c, 0x00,
 0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x00,
 0x78, 0xcc, 0x0c, 0x38, 0x60, 0xcc, 0xfc, 0x00,
 0x78, 0xcc, 0x0c, 0x38, 0x0c, 0xcc, 0x78, 0x00,
 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x1e, 0x00,
 0xfc, 0xc0, 0xf8, 0x0c, 0x0c, 0xcc, 0x78, 0x00,
 0x38, 0x60, 0xc0, 0xf8, 0xcc, 0xcc, 0x78, 0x00,
 0xfc, 0xcc, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x00,
 0x78, 0xcc, 0xcc, 0x78, 0xcc, 0xcc, 0x78, 0x00,
 0x78, 0xcc, 0xcc, 0x7c, 0x0c, 0x18, 0x70, 0x00,
 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00,
 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60,
 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x00,
 0x00, 0x00, 0xfc, 0x00, 0x00, 0xfc, 0x00, 0x00,
 0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00,
 0x78, 0xcc, 0x0c, 0x18, 0x30, 0x00, 0x30, 0x00,
 0x7c, 0xc6, 0xde, 0xde, 0xde, 0xc0, 0x78, 0x00,
 0x30, 0x78, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0x00,
 0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00,
 0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 0x00,
 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00,
 0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 0x00,
 0xfe, 0x62, 0x68, 0x78, 0x68, 0x60, 0xf0, 0x00,
 0x3c, 0x66, 0xc0, 0xc0, 0xce, 0x66, 0x3e, 0x00,
 0xcc, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0xcc, 0x00,
 0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,
 0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00,
 0xe6, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 0x00,
 0xf0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00,
 0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x00,
 0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x00,
 0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00,
 0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00,
 0x78, 0xcc, 0xcc, 0xcc, 0xdc, 0x78, 0x1c, 0x00,
 0xfc, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0xe6, 0x00,
 0x78, 0xcc, 0xe0, 0x70, 0x1c, 0xcc, 0x78, 0x00,
 0xfc, 0xb4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,
 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfc, 0x00,
 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00,
 0xc6, 0xc6, 0xc6, 0xd6, 0xfe, 0xee, 0xc6, 0x00,
 0xc6, 0xc6, 0x6c, 0x38, 0x38, 0x6c, 0xc6, 0x00,
 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x30, 0x78, 0x00,
 0xfe, 0xc6, 0x8c, 0x18, 0x32, 0x66, 0xfe, 0x00,
 0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00,
 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x00,
 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00,
 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
 0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00,
 0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xdc, 0x00,
 0x00, 0x00, 0x78, 0xcc, 0xc0, 0xcc, 0x78, 0x00,
 0x1c, 0x0c, 0x0c, 0x7c, 0xcc, 0xcc, 0x76, 0x00,
 0x00, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00,
 0x38, 0x6c, 0x60, 0xf0, 0x60, 0x60, 0xf0, 0x00,
 0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8,
 0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xe6, 0x00,
 0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,
 0x0c, 0x00, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78,
 0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00,
 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,
 0x00, 0x00, 0xcc, 0xfe, 0xfe, 0xd6, 0xc6, 0x00,
 0x00, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0xcc, 0x00,
 0x00, 0x00, 0x78, 0xcc, 0xcc, 0xcc, 0x78, 0x00,
 0x00, 0x00, 0xdc, 0x66, 0x66, 0x7c, 0x60, 0xf0,
 0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0x1e,
 0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 0xf0, 0x00,
 0x00, 0x00, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x00,
 0x10, 0x30, 0x7c, 0x30, 0x30, 0x34, 0x18, 0x00,
 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00,
 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00,
 0x00, 0x00, 0xc6, 0xd6, 0xfe, 0xfe, 0x6c, 0x00,
 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00,
 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8,
 0x00, 0x00, 0xfc, 0x98, 0x30, 0x64, 0xfc, 0x00,
 0x1c, 0x30, 0x30, 0xe0, 0x30, 0x30, 0x1c, 0x00,
 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00,
 0xe0, 0x30, 0x30, 0x1c, 0x30, 0x30, 0xe0, 0x00,
 0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x10, 0x38, 0x6c, 0xc6, 0xc6, 0xfe, 0x00,
};
 #asm
.org 0xcff0
bios_table_area_end:
 #endasm
