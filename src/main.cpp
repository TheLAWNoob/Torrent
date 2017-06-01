#include "alib.h"
#include "atype.h"
#include "ssl_fn.h"
#include "log.h"
#include "lzma_wrapper.h"

#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Test if ssl_fn.c create_sha1sum is working correctly
 *
 */
void sha1_test()
{
    unsigned char *tmp = NULL;
    tmp = create_sha1sum((char *)"sha1.txt");
    if(tmp != NULL) {
        for (int i = 0; i < 20; i++) {
            printf("%02x",tmp[i]);
        }
        printf("\n");
        free(tmp);
    }
}

/* Test if log.c log_msg is working correctly
 *
 */
void log_test()
{
    log_msg("fewf %s\n", "wfean");
}

void chain_test()
{
    chain *ch = newChain();
    uint32_t size = 1000;

    for (uint32_t i = 0; i < size; i++) {
        uint64_t key = 0xFFFF0000FFFF0000 + i;
        uint32_t len = 4;
        pack **packs = (pack **)malloc(sizeof(pack *) * len);
        if (packs == NULL)
            break;
        for (uint32_t j = 0; j < len; j++) {
            packs[j] = newPack((char *)"dn:testtesttesttesttesttesttesttest",
                               4*1024*1024,
                               (char *)"xt:testtesttesttesttesttesttesttest",
                               (char *)"tr:testtesttesttesttesttesttesttest");
        }

        if (!insertBlock(newBlock(i, key, len, packs), ch))
            break;
    }
    printBlock(ch->head[0]);
    printBlock(ch->head[size - 1]);

    printf("Free'd %lu bytes\n", deleteChain(ch) + sizeof(chain));

    free(ch);
}

void zip_test()
{
    compress_file("t2","t2.my7z", NULL);
}

chain *chain_gen(uint64_t size)
{
    uint64_t i;
    uint16_t j, k, nPack;
    const char charset[] = "qazwsxedcrfvtgbyhnujmikolpQAZWSXEDCRFVTGBYHNUJMIKOLP0123456789";//62
    
    uint64_t key;
    chain *ch = newChain();
    char *dn = (char *)malloc(sizeof(char) * 121);
    
    for (i = 0; i < size && i < MAX_U32; i++) {
        nPack  = rand() % 50 + 50;
        pack **packs = (pack **)malloc(sizeof(pack *) * nPack);
        
        for (j = 0; j < nPack; j++) {
            k = rand() % 90 + 30;
            dn[k] = 0;
            for (k--; k > 0; k--) {
                dn[k] = charset[rand()%62];
            }
            dn[0] = '0';
            
            packs[j] = newPack(dn, (rand()%50+1)*1024*1024, dn, dn);
        }
        
        key = rand() % MAX_U16 * MAX_U32;
        if (!insertBlock(newBlock((uint32_t)i, key, nPack, packs), ch))
            break;
    }
    free(dn);
    return ch;
}
void uncompress_test() {
    for (int i = 1; i < 6; i++) {
        char buff1[64], buff2 [64];
        sprintf(buff1,"temp%d.7z", i);
        sprintf(buff2, "temp%d.unc",i);
        decompress_file(buff1, buff2, NULL);
    }
}

int main()
{
//    sha1_test();
//    log_test();
//    chain_test();
//    zip_test();
    printf("\nGenerating\n");
    chain *ch = chain_gen(3000);
    
    printf("Compressing\n");
    struct timespec tmp1,tmp2;
    clock_gettime(CLOCK_MONOTONIC, &tmp1);
    chainCompactor(ch, 5);
    clock_gettime(CLOCK_MONOTONIC, &tmp2);
    uint32_t tmp = (tmp2.tv_sec - tmp1.tv_sec) * 1000 + (tmp2.tv_nsec - tmp1.tv_nsec) / 1000000;
    printf("Took %d milliseconds\n", tmp);
    
    
    printf("\nFree'd %lu bytes\n", deleteChain(ch) + sizeof(chain));
    free(ch);
    
    //std::cout.imbue(std::locale());
    uncompress_test();
    return 0;
}

