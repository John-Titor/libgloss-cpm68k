/*
 * Convert a relocatable elf32-m68k program to relocatable CP/M-68k format.
 * 
 * Compile with gcc/clang: cc -Wall elf2cpm68k.c
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>

#define MAX_RELOCS      5000        /* increase as necessary */
#define COPY_BUF        1024        /* increase if stack is not an issue */

typedef uint8_t field8;
typedef struct { uint8_t bytes[2]; } field16;
typedef struct { uint8_t bytes[4]; } field32;

#define READ16(_f)  (((uint32_t)((_f).bytes[0]) << 8) \
                    + (_f).bytes[1])
#define READ32(_f)  (((uint32_t)((_f).bytes[0]) << 24) \
                    + ((uint32_t)((_f).bytes[1]) << 16) \
                    + ((uint32_t)((_f).bytes[2]) << 8) \
                    + (_f).bytes[3])

#define WRITE16(_f, _v) { (_f).bytes[0] = ((_v) >> 8) & 0xff; \
                          (_f).bytes[1] = (_v) & 0xff; }

#define WRITE32(_f, _v) { (_f).bytes[0] = ((_v) >> 24) & 0xff; \
                          (_f).bytes[1] = ((_v) >> 16) & 0xff; \
                          (_f).bytes[2] = ((_v) >> 8) & 0xff; \
                          (_f).bytes[3] = (_v) & 0xff; }

typedef struct {
    uint8_t     e_ident[0x10];
#define EI_MAG0     0
#define EI_MAG1     1
#define EI_MAG2     2
#define EI_MAG3     3
#define EI_CLASS    4
#define ELFCLASS32      1
#define EI_DATA     5
#define ELFDATA2MSB     2
#define EI_VERSION  6
    field16     e_type;
#define ET_EXEC         2
    field16     e_machine;
#define EM_68K          4
    field32     e_version;
    field32     e_entry;
    field32     e_phoff;
    field32     e_shoff;
    field32     e_flags;
    field16     e_ehsize;
    field16     e_phentsize;
    field16     e_phnum;
    field16     e_shentsize;
    field16     e_shnum;
    field16     e_shstrndx;
} elf32_file_header_t;

#define e_type(_h)      READ16((_h).e_type)
#define e_machine(_h)   READ16((_h).e_machine)
#define e_version(_h)   READ32((_h).e_version)
#define e_entry(_h)     READ32((_h).e_entry)
#define e_phoff(_h)     READ32((_h).e_phoff)
#define e_shoff(_h)     READ32((_h).e_shoff)
#define e_flags(_h)     READ32((_h).e_flags)
#define e_ehsize(_h)    READ16((_h).e_ehsize)
#define e_phentsize(_h) READ16((_h).e_phentsize)
#define e_phnum(_h)     READ16((_h).e_phnum)
#define e_shentsize(_h) READ16((_h).e_shentsize)
#define e_shnum(_h)     READ16((_h).e_shnum)
#define e_shstrndx(_h)  READ16((_h).e_shstrndx)

typedef struct {
    field32     p_type;
#define PT_LOAD         1
    field32     p_offset;
    field32     p_vaddr;
    field32     p_paddr;
    field32     p_filesz;
    field32     p_memsz;
    field32     p_flags;
#define PF_X    (1 << 0)
#define PF_W    (1 << 1)
#define PF_R    (1 << 2)
#define PF_MASK (PF_X | PF_W | PF_R)
    field32     p_align;
} elf32_program_header_t;

#define p_type(_h)      READ32((_h).p_type)
#define p_offset(_h)    READ32((_h).p_offset)
#define p_vaddr(_h)     READ32((_h).p_vaddr)
#define p_paddr(_h)     READ32((_h).p_paddr)
#define p_filesz(_h)    READ32((_h).p_filesz)
#define p_memsz(_h)     READ32((_h).p_memsz)
#define p_flags(_h)     READ32((_h).p_flags)
#define p_align(_h)     READ32((_h).p_align)

typedef struct {
    field32     sh_name;
    field32     sh_type;
#define SHT_RELA          4
#define SHT_REL           9
    field32     sh_flags;
#define SHF_ALLOC            (1 << 1)
    field32     sh_addr;
    field32     sh_offset;
    field32     sh_size;
    field32     sh_link;
    field32     sh_info;
    field32     sh_addralign;
    field32     sh_entsize;
} elf32_section_header_t;

#define sh_name(_h)     READ32((_h).sh_name)
#define sh_type(_h)     READ32((_h).sh_type)
#define sh_flags(_h)    READ32((_h).sh_flags)
#define sh_addr(_h)     READ32((_h).sh_addr)
#define sh_offset(_h)   READ32((_h).sh_offset)
#define sh_size(_h)     READ32((_h).sh_size)
#define sh_link(_h)     READ32((_h).sh_link)
#define sh_info(_h)     READ32((_h).sh_info)
#define sh_addralign(_h) READ32((_h).sh_addralign)
#define sh_entsize(_h)  READ32((_h).sh_entsize)

typedef struct {
    field32     rel_offset;
    field32     rel_info;
    field32     rel_addend;
} elf32_rela_t;

#define rel_offset(_h)  READ32((_h).rel_offset)
#define rel_info(_h)    READ32((_h).rel_info)
#define rel_addend(_h)  READ32((_h).rel_addend)

#define REL_SYM(_info)           ((_info) >> 8)
#define REL_TYPE(_info)          ((_info) & 0xff)
#define R_68K_NONE      0
#define R_68K_32        1
#define R_68K_PC32      4

typedef struct {
    field32     st_name;
    field32     st_value;
    field32     st_size;
    uint8_t          st_info;
    uint8_t          st_other;
    field16     st_shndx;
} elf32_sym_t;

#define st_name(_h)     READ32((_h).st_name)
#define st_value(_h)    READ32((_h).st_value)
#define st_size(_h)     READ32((_h).st_size)
#define st_shndx(_h)    READ16((_h).st_shndx)

typedef struct {
    FILE        *fp;

    /* in-memory loaded file contents */
    elf32_file_header_t     header;
    elf32_program_header_t  *segments;
    elf32_section_header_t  *sections;

    void        **section_data;

    elf32_section_header_t  *symtab;

    /* relevant things derived from file contents */
    uint32_t    text_offset;
    uint32_t    text_filesize;

    uint32_t    data_offset;
    uint32_t    data_vmaddr;
    uint32_t    data_filesize;

    uint32_t    bss_memsize;

    uint32_t    rel_count;
    uint32_t    rel_data[MAX_RELOCS];
#define REL_TYPE_SHIFT  28
#define REL_TYPE_DATA   (1LU << REL_TYPE_SHIFT)
#define REL_TYPE_TEXT   (2LU << REL_TYPE_SHIFT)
#define REL_TYPE_BSS    (3LU << REL_TYPE_SHIFT)
#define REL_TYPE_MASK   (3LU << REL_TYPE_SHIFT)
} elf_file_t;

void
error(const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "ERROR: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    exit(1);
}

void *
xcalloc(size_t size, size_t count)
{
    void *result = malloc(size * count);

    if (result == NULL) {
        error("failed to allocate %ld items of %d bytes", (long)count, (long)size);
    }
    memset(result, 0, size * count);
    return result;
}

void
read_at(const char *what, FILE *fp, void *buffer, long where, size_t count)
{
    size_t actual;

    if (where >= 0) {
        fseek(fp, where, SEEK_SET);
    } else {
        where = ftell(fp);
    }
    actual = fread(buffer, 1, count, fp);

    if (actual != count) {
        error("reading %s: %s", what, strerror(errno));
    }
}

elf32_section_header_t *
elf_section_header(elf_file_t *ef, uint32_t index) {
    if (index >= e_shnum(ef->header)) {
        error("reference to invalid section %ld", index);
    }
    return ef->sections + index;
}

void *
elf_section_data(elf_file_t *ef, uint32_t index)
{
    elf32_section_header_t  *sp = ef->sections + index;
    void                    **bufp = ef->section_data + index;

    if (*bufp == NULL) {
        *bufp = xcalloc(sh_size(*sp), 1);
        read_at("section data", ef->fp, *bufp, sh_offset(*sp), sh_size(*sp));
    }
    return *bufp;
}

void
load_elf_relocs(elf_file_t *ef)
{
    uint32_t section_index;
    uint32_t rel_index;

    /* scan sections looking for relocation things */
    for (section_index = 0; section_index < e_shnum(ef->header); section_index++) {
        elf32_section_header_t *sh = elf_section_header(ef, section_index);
        elf32_rela_t *rels;
        uint32_t num_rels;
        size_t rel_size;
        elf32_sym_t *symtab;

        /* check section type for relocations */
        switch (sh_type(*sh)) {
        case SHT_RELA:
            break;
        case SHT_REL:   /* we only expect RELA relocations */
            error("not expecting SHT_REL");
        default:
            /* not a relocation information section */
            continue;
        }

        /* check section referenced by relocations, ignore if not loaded */
        if (!(sh_flags(*elf_section_header(ef, sh_info(*sh))) & SHF_ALLOC)) {
            continue;
        }

        /* load section data */
        rels = (elf32_rela_t *)elf_section_data(ef, section_index);
        symtab = (elf32_sym_t *)elf_section_data(ef, sh_link(*sh));

        /* scan relocations */
        rel_size = sh_entsize(*sh);
        if (rel_size == 0) {
            error("relocation section entry size is zero");
        }
        num_rels = sh_size(*sh) / rel_size;
        for (rel_index = 0; rel_index < num_rels; rel_index++) {
            elf32_rela_t *rel = rels + rel_index;
            uint32_t rel_offset = rel_offset(*rel);
            uint32_t rel_type = REL_TYPE(rel_info(*rel));
            elf32_sym_t *rel_sym = symtab + REL_SYM(rel_info(*rel));
            uint32_t rel_target = st_value(*rel_sym);
            uint32_t rel_target_type = 0;

            /* check whether this relocation is interesting */
            switch (rel_type) {
            case R_68K_32:
                if ((ef->rel_count + 1) >= MAX_RELOCS) {
                    error("too many relocations (increase MAX_RELOCS and recompile)");
                }
                break;
            case R_68K_PC32:    /* these appear in .eh_frame and can be ignored */
            case R_68K_NONE:    /* these should only appear in debug info */
                continue;
            default:
                error("unsupported relocation type %ld", rel_type);
            }
            /* work out what the relocated pointer refers to */
            if (rel_target < ef->text_filesize) {
                rel_target_type = REL_TYPE_TEXT;
            } else if ((rel_target >= ef->data_vmaddr)
                       && (rel_target <= (ef->data_vmaddr + ef->data_filesize))) {
                rel_target_type = REL_TYPE_DATA;
            } else if (rel_target <= (ef->data_vmaddr + ef->data_filesize + ef->bss_memsize)) {
                rel_target_type = REL_TYPE_BSS;
            } else {
                error("relocation at 0x%lx not pointing to text/data/bss (0x%lx)",
                      rel_offset, rel_target);
            }

            if (rel_offset & REL_TYPE_MASK) {
                error("program too large to encode relocation %lx", rel_offset);
            }

            ef->rel_data[ef->rel_count++] = rel_offset | rel_target_type;
/*            fprintf(stderr, "rel: 0x%lx\n", rel_offset | rel_target_type); */
        }
    }
    for (rel_index = 1; rel_index < ef->rel_count; rel_index++) {
        uint32_t prev_rel = ef->rel_data[rel_index - 1] & ~REL_TYPE_MASK;
        uint32_t this_rel = ef->rel_data[rel_index] & ~REL_TYPE_MASK;
        if (this_rel < prev_rel) {
            error("relocations not sorted");
        }
    }
}

void
load_elf(elf_file_t *ef)
{
    elf32_program_header_t           *text_segment;
    elf32_program_header_t           *data_segment;

    read_at("ELF header", ef->fp, &ef->header, 0, sizeof(ef->header));
    if ((ef->header.e_ident[EI_MAG0] != 0x7f)
        || (ef->header.e_ident[EI_MAG1] != 'E')
        || (ef->header.e_ident[EI_MAG2] != 'L')
        || (ef->header.e_ident[EI_MAG3] != 'F')
        || (ef->header.e_ident[EI_CLASS] != ELFCLASS32)
        || (ef->header.e_ident[EI_DATA] != ELFDATA2MSB)
        || (ef->header.e_ident[EI_VERSION] != 1)
        || (e_type(ef->header) != ET_EXEC)
        || (e_machine(ef->header) != EM_68K)
        || (e_version(ef->header) != 1)
        || (e_ehsize(ef->header) != sizeof(elf32_file_header_t))
        || (e_phentsize(ef->header) != sizeof(elf32_program_header_t))
        || (e_shentsize(ef->header) != sizeof(elf32_section_header_t))
        || (e_shnum(ef->header) < 2)) {
        error("bad ELF header");
    }

    /* read and cache segment / section data */
    ef->segments = xcalloc(e_phentsize(ef->header), e_phnum(ef->header));
    ef->sections = xcalloc(e_shentsize(ef->header), e_shnum(ef->header));
    ef->section_data = xcalloc(sizeof(void *), e_shnum(ef->header));

    read_at("segment headers", 
            ef->fp, ef->segments,
            e_phoff(ef->header),
            e_phentsize(ef->header) * e_phnum(ef->header));
    read_at("section headers", 
            ef->fp, ef->sections,
            e_shoff(ef->header),
            e_shentsize(ef->header) * e_shnum(ef->header));

    /* expect segment[0] to be text, segment[1] to be data */
    text_segment = ef->segments + 0;
    data_segment = ef->segments + 1;

    /* parse text segment and validate assumptions */
    ef->text_offset = p_offset(*text_segment);
    ef->text_filesize = p_filesz(*text_segment);
    if ((p_type(*text_segment) != PT_LOAD)
        || ((p_flags(*text_segment) & PF_MASK) != (PF_X | PF_R))
        || (p_vaddr(*text_segment) != 0)
        || (p_memsz(*text_segment) != ef->text_filesize)) {
        error("bad text segment (type, flags, p/v size or address)");
    }

    /* parse data segment and validate assumptions */
    ef->data_offset = p_offset(*data_segment);
    ef->data_vmaddr = p_vaddr(*data_segment);
    ef->data_filesize = p_filesz(*data_segment);
    ef->bss_memsize = p_memsz(*data_segment) - ef->data_filesize;
    if ((p_type(*data_segment) != PT_LOAD)
        || ((p_flags(*data_segment) & PF_MASK) != (PF_W | PF_R))
        || (ef->data_vmaddr < ef->text_filesize)) {
        error("bad data segment (type, flags, p/v size or address)");
    }

    load_elf_relocs(ef);

    printf("Loaded ELF file: text 0x%x data 0x%x bss 0x%x, %u relocations\n",
           ef->text_filesize, ef->data_filesize, ef->bss_memsize, ef->rel_count);
}

typedef struct {
    field16     fh_type;
#define FH_CONTIG       0x601a
//#define FH_NONCONTIG    0x601b
    field32     fh_textlen;
    field32     fh_datalen;
    field32     fh_bsslen;
    field32     fh_symlen;
    field32     fh_pad;
    field32     fh_entry;
    field16     fh_flags;
#define FH_NO_RELOC     0xffff
#define FH_WITH_RELOC   0x0000
//    field32     fh_data_addr;
//    field32     fh_bss_addr;
} cpm_exec_header_t;

void
write_out(const char *what, FILE *fp, void *buffer, size_t count)
{
    if (fwrite(buffer, 1, count, fp) != count) {
        error("writing %s: %s", what, strerror(errno));
    }
}

void
copy_bytes(const char *what, FILE *source, long source_offset, FILE *destination, size_t count) 
{
    uint8_t      copy_buffer[COPY_BUF];

    while (count > 0) {
        size_t resid = (count < COPY_BUF) ? count : COPY_BUF;

        read_at(what, source, copy_buffer, source_offset, resid);
        source_offset += resid;
        write_out(what, destination, copy_buffer, resid);
        count -= resid;
    }
}

void
zerofill(const char *what, FILE *destination, size_t count)
{
    uint8_t  zero = 0;

    while (count--) {
        if (fwrite(&zero, 1, 1, destination) < 0) {
            error("writing %s: %s", strerror(errno));
        }
    }
}

void
write_cpm_relocs(elf_file_t *ef, FILE *cfp)
{
    uint32_t    current_vma = 0;
    uint32_t    rel_index = 0;
    field16     relword;

    while (rel_index < ef->rel_count) {
        uint32_t next_rel_addr = ef->rel_data[rel_index] & ~REL_TYPE_MASK;
        uint32_t next_rel_type = (ef->rel_data[rel_index] & REL_TYPE_MASK) >> REL_TYPE_SHIFT;

        /* pad the end of the text segment if necessary */
        if ((current_vma < ef->data_vmaddr)
            && (next_rel_addr >= ef->data_vmaddr)) {
            zerofill("relocation padding", cfp, ef->text_filesize - current_vma);
            current_vma = ef->data_vmaddr;
        }
        zerofill("relocation padding", cfp, next_rel_addr - current_vma);
        current_vma = next_rel_addr;
        WRITE16(relword, 5);
        write_out("relocation", cfp, &relword, sizeof(relword));
        WRITE16(relword, next_rel_type);
        write_out("relocation", cfp, &relword, sizeof(relword));
        rel_index++;
        current_vma+=4;
        /* XXX */
    }
    /* pad to the end of the file */
    zerofill("relocation padding", cfp, ef->data_vmaddr + ef->data_filesize - current_vma);
}

void
write_cpm(elf_file_t *ef, FILE *cfp)
{
    cpm_exec_header_t   ch;

    /* segments must always be 16b-size-aligned */
    uint32_t text_pad = ef->text_filesize % 2;
    uint32_t data_pad = ef->data_filesize % 2;
    uint32_t bss_pad = ef->bss_memsize % 2;

    WRITE16(ch.fh_type, FH_CONTIG);
//    WRITE16(ch.fh_type, FH_NONCONTIG);
    WRITE32(ch.fh_textlen, ef->text_filesize + text_pad);
    WRITE32(ch.fh_datalen, ef->data_filesize + data_pad);
    WRITE32(ch.fh_bsslen, ef->bss_memsize + bss_pad);
    WRITE32(ch.fh_symlen, 0);
    WRITE32(ch.fh_pad, 0);
    WRITE32(ch.fh_entry, 0);    /* XXX ??? */
    WRITE16(ch.fh_flags, ef->rel_count ? FH_WITH_RELOC : FH_NO_RELOC);
//    WRITE32(ch.fh_data_addr, ef->data_vmaddr);
//    WRITE32(ch.fh_bss_addr, ef->data_vmaddr + ef->data_filesize);

    write_out("CP/M-68k exec header", cfp, &ch, sizeof(ch));

    copy_bytes("text segment", ef->fp, ef->text_offset, cfp, ef->text_filesize);
    if (text_pad > 0) {
        zerofill("text padding", cfp, text_pad);
        ef->text_filesize += text_pad;
    }
    copy_bytes("data segment", ef->fp, ef->data_offset, cfp, ef->data_filesize);
    if (data_pad > 0) {
        zerofill("data padding", cfp, data_pad);
        ef->data_filesize += data_pad;
    }
    write_cpm_relocs(ef, cfp);
}

int
main(int argc, const char *argv[])
{
    FILE        *cfp;
    elf_file_t  *ef;

    if (argc != 3) {
        error("usage: elf2cpm68k <input file> <output file>");
    }

    ef = (elf_file_t *)xcalloc(sizeof(elf_file_t), 1);
    if ((ef->fp = fopen(argv[1], "rb")) == NULL) {
        error("can't open ELF file %s: %s", argv[1], strerror(errno));
    }
    load_elf(ef);

    if ((cfp = fopen(argv[2], "wb")) == NULL) {
        error("can't open CP/M-68K file %s: %s", argv[2], strerror(errno));
    }
    write_cpm(ef, cfp);
    fclose(ef->fp);
    if (fclose(cfp) < 0) {
        error("flushing CP/M-68k file: %s", strerror(errno));
    }

    exit(0);
}
