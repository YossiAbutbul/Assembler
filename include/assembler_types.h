/**
 * @file assembler_types.h
 * @brief Core data structure definitions for the assembler.
 *
 * This file contains all shared data structures used across the assmebler.
 */
#ifndef ASSEMBLER_TYPES_H
#define ASSEMBLER_TYPES_H

#include "constants.h"

/* === Forward declarations === */
struct Symbol;
struct Instruction;
struct Operand;

/* === Asswmbly Output Data Structure === */

/**
 * @brief Node for tracking .entry symbols during assembly.
 *
 * Used to build linked list of all symbols declared as .entry
 * for outpput file genration.
 */
typedef struct ExternalNode
{
    char name[MAX_SYMBOL_NAME_LENGTH]; /* Symbol name */
    int address;                       /* Symbol address in memory */
    struct EntryNode *next;            /* Pointer to next entry node */
} EntryNode;

/**
 * @brief Node for tracking external symbols refernces during assembly.
 *
 * Used to build a linked list of all locations where external symbols
 * are referenced for output file generation.
 */
typedef struct ExternalNode
{
    char name[MAX_SYMBOL_NAME_LENGTH]; /* External symbol name */
    int address;                       /* Address where symbol is referenced in memory */
    struct ExternalNode *next;         /* Pointer to next external node */
} ExternalNode;

/**
 * @brief Dynamic storage for instruction machine code.
 *
 * Stores and genrated machine code for all instructions with their
 * corresponding memory addresses.
 */
typedef struct InstructionImage
{
    int *code;      /* Store instruction machine code */
    int *addresses; /* Store corresponding addresses for each instruction word */
    int size;       /* Current number of stored words */
    int capacity;   /* Maximum capacity before reallocation */
} InstructionImage;

/* === Assembler Processing Context === */

/**
 * @brief Complete assembly context containg all processing state.
 *
 * This structure aggregates all the data needed during assembly
 * processing and can be passed between modules as needed.
 */
typedef struct AssemblyContext
{
    InstructionImage *instruction_image; /*Generated instruction code */
    EntryNode *entry_list;               /* List of .entry symbols */
    ExternalNode *external_list;         /* List of external refrences */
    int IC;                              /* Current instruction counter */
    int DC;                              /* Current data counter */
    int ICF;                             /* Final instruction counter */
    int DCF;                             /* Final Data counter */
    BOOL has_errors;                     /* Error flag for assembly process */
} AssemblyContext;

/* === File Processing Results === */

/**
 * @brief Results from assembly file processing.
 *
 * Contains all the information needed to genrate output files
 * after successful assembly.
 */
typedef struct AssemblyResults
{
    AssemblyContext *context; /* Assembly processing context */
    char *source_filename;    /* Original source file name */
    BOOL success;             /* TRUE if assembly succeeded */
} AssemblyResults;

#endif