# Style guide

## Comments

Comments can be placed at the end of a line (addresses the whole line).
Comments can be placed above line(s) (addresses all following line(s) until
next empty line).
Comments over multiple lines have the following style (not applicable for hard
wrapped comments at the end of lines):

```C
/* First comment line
 * Further comment line(s)
 */
```

Far away closing brackets of nested constructs are commented at the end to
indicate to what they belong:

`} /* for(...) */`

`} /* while(...) */`

`#endif /* INCLUDE_<MODULENAME>_H */`

Modules are structured into an attributes and operations section with:

```C
/* ATTRIBUTES/OPERATIONS
 * =====================
 */
```

## Applicaable documents

Applicable guidelines and standards are: MISRA-C:2012

## Naming

> Characters between `<` and `>` are placeholders and need to be replaced.

**Prefixes**

> All in caps for defines.

Types:

* `b_` (bool)
* `p_` (pointer)
* `u8_` (uint8_t)
* `u16_` (uint16_t)
* `u32_` (uint32_t)
* `en_` (enumeration)
* `stc_` (struct)

Scope:

* `g_` (global, non-static)
* `pv_` (global, static)

Functions: `fn_`

Include guards: `INCLUDE_`

> Scope prefixes come after all type prefixes.

**Names**

Files: `<moduleName.h>`/`<moduleName.c>`

Include guards: `<MODULENAME>`

Defines (except include guards): `<MODULENAME>_<FULLNAMEOFDEFINE>`

Modules (without vocals, in function and attributes struct names): `<mdlNm>`

Attributes and operations: `<fullNameOfAttribute>`/`<fullNameOfOperation>`

Variables (also struct members): `<fullNameOfVariable>`

Indices (exception from rule above: without vocals): `idx`/`idx<number>`

Typedefs: `<abbrNmOfT>` (typedefs of attribute struct names use `<mdlNm>` instead)

Enumerations: `<fullNameOfMember>`

Enumeration members: `<FULLNAMEOFMEMBER>`

**Suffixes**

Typedefs: `_t`

Include guards: `_H`

**Name assembly**

`<prefixes_names_postfix`

Include guards: `INCLUDE_<MODULENAME>_H`

Defines (except include guards): `<PREFIXES>_<MODULENAME>_<FULLNAMEOFDEFINE>`

Typedefs: `<prefixes>_<abbrNmOfT>_t` (typedefs of structs don't have prefixes
of their member's types: `stc_<abbrNmOfT>_t`)

me pointer (exception from the rules): `me`

Attributes: `stc_<abbrNmOfT>_<fullNameOfAttribute>`

Operations: `fn_<mdlNm>_<fullNameOfOperation>(<prefixes>_<abbrNmOfT>_t* const me,
                                              <arguments>)`

Variables (execpt indices; also struct members):
`<prefixes>_<fullNameOfVariable>`

Indices (exception from variables): `<prefixes>_idx<number>`

Enumerations: `<prefixes>_<fullNameOfMember>`

Enumeration members (prefixes equal enumeration´s typedef):
`<prefixes>_<FULLNAMEOFMEMBER>`

Examples:

```C
#ifndef INCLUDE_SCHEDULER_H
#define INCLUDE_SCHEDULER_H
```

```C
typedef struct
{
    stc_tskLst_t* p_stc_tskLst_itsTaskList;
}stc_sdlr_t;
```

`stc_sdlr_t stc_sdlr_scheduler;`

`p_stc_tskLst_taskList`

`void fn_sdlr_init(stc_sdlr_t* const me)`

`void fn_sdlr_setItsTaskList(stc_sdlr_t* const me,
                             stc_tskLst_t* const p_stc_tskLst_taskList);`

`const uint8_t u8_taskCount`

`u8_idx2`

```C
typedef enum
{
    en_act_ON,
    en_act_OFF
}en_act_t;
```

`en_act_t en_act_active;`

## Hard wrapping (line breaks)

Set line break at 80 character mark or sooner.

## Indentation

Indent with multiples of 4 spaces.
If long line continues on next line (line wrapping, see above), no hard rule
exist but at least indent by 4 spaces.
Choose something that looks nice and is well readable.
