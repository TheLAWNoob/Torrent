/**
 * @file alib.h
 * @brief Functions that deal with the initialization of
 * the block structure
 *
 * Essentially just cleanup/init functions
 */
#ifndef _ALIB_H
#define _ALIB_H

#include "atype.h"
#include <time.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

//Get current time in seconds
inline time_t sNow();

void restore_block(block *bx,
                   uint32_t time,
                   uint32_t crc,
                   uint16_t *n_packs,
                   uint16_t n_trans,
                   uint32_t n,
                   uint64_t key,
                   pack **packs);

inline void printTime(time_t time);

/**
 * @brief This function will print the relative information of a block
 * 
 * it will print the:
 *	time (when the block was created)
 *	key (Unique identifier)
 *	number of payloads
 */
void printBlock(block *target);

/**
 * @brief Create a new pack (magnet link info)
 * 
 * This fn will allocate a pack struct, and all of its parameters
 * @return NULL - something went wrong :( (malloc failed) \n
 * ptr to new pack struct
 */
bool newPack(pack *px,
             char *dn,   //!< Display name
             uint64_t xl,//!< Exact length (size in bytez)
             char *xt,   //!< exact topic (URN with hash of file)
             char *tr);  //!< tracker url

void newTran(tran *tx);

void newBlock(block *bx,
              uint32_t n,
              uint64_t key,
              uint32_t *n_packs,//address of package count
              pack **packs);    //address of package array

chain *newChain(void);

//! return 1 on success
bool insertBlock(block *bx, chain *ch);

void deletePack(pack *target);

void deleteChain(chain *target);

#endif //_ALIB_H

