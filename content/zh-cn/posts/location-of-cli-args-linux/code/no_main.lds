ENTRY(entrypoint)

SECTIONS {
    . = 0x8048000 + SIZEOF_HEADERS;

    .CODE : {
        *(.init) *(.text) *(.fini)
    }

    .DATA : {
        *(.data)
        i = .;
    }

    /DISCARD/ : {
        *(.comment)
    }
}
