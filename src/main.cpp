#include "atype.h"
#include "alib.h"
#include "alibio.h"
#include "ssl_fn.h"
#include "log.h"
#include "lzma_wrapper.h"

#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define N_THREADS 5
#define N_TEST_BLOCKS 9000

/* Test if log.c log_msg is working correctly
 *
 */
void log_test()
{
	log_msg("fewf %s\n", "wfean");
}

void zip_test()
{
	compress_file("t2", "t2.my7z", NULL);
}

chain *chain_gen(uint64_t size)
{
//#define rand() (33)
	uint16_t j, k;
	uint32_t nPack;
	uint64_t i;
	const char charset[] = "qazwsxedcrfvtgbyhnujmikolpQAZWSXEDCRFVTGBYHNUJMIKOLP0123456789";//62

	uint64_t key;
	block bx;
	chain *ch = newChain();
	char *dn = (char *) malloc(sizeof(char) * 121);

	for (i = 0; i < size && i < B_SUM; i++) {
		nPack = rand() % 50 + 50;
		pack *packs = (pack *) malloc(sizeof(pack) * nPack);

		for (j = 0; j < nPack; j++) {
			k = rand() % 90 + 30;
			dn[k] = 0;
			for (k--; k > 0; k--) {
				dn[k] = charset[rand() % 62];
			}
			dn[0] = '0';

			bool val = newPack(&packs[j], dn, (rand() % 50 + 1) * 1024 * 1024, dn, dn);
		}

		key = rand() % MAX_U16 * MAX_U32;
		newBlock(&bx, (uint32_t) i, key, &nPack, &packs);
		if (!insertBlock(&bx, ch)) break;
	}
	free(dn);
	return ch;
}

typedef struct {
	char in7z[64];
	char outf[64];
} decompParams;

void *decompress_wrap(void *args)
{
	decompParams *dp = (decompParams *) args;
	decompress_file(dp->in7z, dp->outf);
	return NULL;
}

void uncompress_test()
{
	pthread_t threads[N_THREADS];
	decompParams dp[N_THREADS];

	for (int i = 0; i < N_THREADS; i++) {
		sprintf(dp[i].in7z, "temp%d.file.7z", i + 1);
		sprintf(dp[i].outf, "temp%d.1unc", i + 1);
		pthread_create(&threads[i], NULL, &decompress_wrap,
					   (void *) &dp[i]);
	}
	for (int i = 0; i < N_THREADS; i++)
		pthread_join(threads[i], NULL);
}

void chain_test()
{
	printf("\nGenerating\n");
	chain *ch = chain_gen(N_TEST_BLOCKS);

	printf("Compressing\n");
	uint32_t tmp;
#ifdef _WIN32
	tmp = GetTickCount();
	chainCompactor(ch, N_THREADS);
	tmp = GetTickCount() - tmp;
#else
	struct timespec tmp1, tmp2;
	clock_gettime(CLOCK_MONOTONIC, &tmp1);	//Start
	chainCompactor(ch, N_THREADS);
	clock_gettime(CLOCK_MONOTONIC, &tmp2);	//End
	tmp = (tmp2.tv_sec - tmp1.tv_sec) * 1000
            + (tmp2.tv_nsec - tmp1.tv_nsec) / 1000000;
#endif
	printf("Took %d milliseconds\n", tmp);

	deleteChain(ch);
	printf("\nMemory free'd");
	free(ch);

	uncompress_test();
}

int main()
{
//    log_test();
//    zip_test();
	chain_test();

	//std::cout.imbue(std::locale());//Might be useful to remove valgrind false positives
	return 0;
}

