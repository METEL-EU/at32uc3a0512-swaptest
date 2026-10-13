#include <avr32/io.h>
#include <stdio.h>
#include <stdarg.h>
#include "debug.h"
#define TPRINTF(...) dprintf(__VA_ARGS__)

#include "user_board.h"
#include "cycle_counter.h"
#include "spi_flash.h"
#include "boot.h"
#include "idblock.h"
#include "fw_id.h"
#define TEST_DELAY_MS(ms) cpu_delay_ms((ms), FCPU)

#define TABLE_COUNT       24
#define CHECKS_PER_TABLE  20
#define TESTS_PER_ITER    (TABLE_COUNT * CHECKS_PER_TABLE)

typedef int (*fn_t)(void);

extern int refA(void);
extern int refB(void);
extern int refC(void);
extern int refD(void);
extern int refE(void);
extern int refF(void);
extern int refG(void);
extern int refH(void);
extern int refI(void);
extern int refJ(void);
extern int refK(void);
extern int refL(void);
extern int refM(void);
extern int refN(void);
extern int refO(void);
extern int refP(void);
extern int refQ(void);
extern int refR(void);
extern int refS(void);
extern int refT(void);
extern int refU(void);
extern int refV(void);
extern int refW(void);
extern int refX(void);

extern int checkA0(void);
extern int checkA1(void);
extern int checkA2(void);
extern int checkA3(void);
extern int checkA4(void);
extern int checkA5(void);
extern int checkA6(void);
extern int checkA7(void);
extern int checkA8(void);
extern int checkA9(void);
extern int checkA10(void);
extern int checkA11(void);
extern int checkA12(void);
extern int checkA13(void);
extern int checkA14(void);
extern int checkA15(void);
extern int checkA16(void);
extern int checkA17(void);
extern int checkA18(void);
extern int checkA19(void);
extern int checkB0(void);
extern int checkB1(void);
extern int checkB2(void);
extern int checkB3(void);
extern int checkB4(void);
extern int checkB5(void);
extern int checkB6(void);
extern int checkB7(void);
extern int checkB8(void);
extern int checkB9(void);
extern int checkB10(void);
extern int checkB11(void);
extern int checkB12(void);
extern int checkB13(void);
extern int checkB14(void);
extern int checkB15(void);
extern int checkB16(void);
extern int checkB17(void);
extern int checkB18(void);
extern int checkB19(void);
extern int checkC0(void);
extern int checkC1(void);
extern int checkC2(void);
extern int checkC3(void);
extern int checkC4(void);
extern int checkC5(void);
extern int checkC6(void);
extern int checkC7(void);
extern int checkC8(void);
extern int checkC9(void);
extern int checkC10(void);
extern int checkC11(void);
extern int checkC12(void);
extern int checkC13(void);
extern int checkC14(void);
extern int checkC15(void);
extern int checkC16(void);
extern int checkC17(void);
extern int checkC18(void);
extern int checkC19(void);
extern int checkD0(void);
extern int checkD1(void);
extern int checkD2(void);
extern int checkD3(void);
extern int checkD4(void);
extern int checkD5(void);
extern int checkD6(void);
extern int checkD7(void);
extern int checkD8(void);
extern int checkD9(void);
extern int checkD10(void);
extern int checkD11(void);
extern int checkD12(void);
extern int checkD13(void);
extern int checkD14(void);
extern int checkD15(void);
extern int checkD16(void);
extern int checkD17(void);
extern int checkD18(void);
extern int checkD19(void);
extern int checkE0(void);
extern int checkE1(void);
extern int checkE2(void);
extern int checkE3(void);
extern int checkE4(void);
extern int checkE5(void);
extern int checkE6(void);
extern int checkE7(void);
extern int checkE8(void);
extern int checkE9(void);
extern int checkE10(void);
extern int checkE11(void);
extern int checkE12(void);
extern int checkE13(void);
extern int checkE14(void);
extern int checkE15(void);
extern int checkE16(void);
extern int checkE17(void);
extern int checkE18(void);
extern int checkE19(void);
extern int checkF0(void);
extern int checkF1(void);
extern int checkF2(void);
extern int checkF3(void);
extern int checkF4(void);
extern int checkF5(void);
extern int checkF6(void);
extern int checkF7(void);
extern int checkF8(void);
extern int checkF9(void);
extern int checkF10(void);
extern int checkF11(void);
extern int checkF12(void);
extern int checkF13(void);
extern int checkF14(void);
extern int checkF15(void);
extern int checkF16(void);
extern int checkF17(void);
extern int checkF18(void);
extern int checkF19(void);
extern int checkG0(void);
extern int checkG1(void);
extern int checkG2(void);
extern int checkG3(void);
extern int checkG4(void);
extern int checkG5(void);
extern int checkG6(void);
extern int checkG7(void);
extern int checkG8(void);
extern int checkG9(void);
extern int checkG10(void);
extern int checkG11(void);
extern int checkG12(void);
extern int checkG13(void);
extern int checkG14(void);
extern int checkG15(void);
extern int checkG16(void);
extern int checkG17(void);
extern int checkG18(void);
extern int checkG19(void);
extern int checkH0(void);
extern int checkH1(void);
extern int checkH2(void);
extern int checkH3(void);
extern int checkH4(void);
extern int checkH5(void);
extern int checkH6(void);
extern int checkH7(void);
extern int checkH8(void);
extern int checkH9(void);
extern int checkH10(void);
extern int checkH11(void);
extern int checkH12(void);
extern int checkH13(void);
extern int checkH14(void);
extern int checkH15(void);
extern int checkH16(void);
extern int checkH17(void);
extern int checkH18(void);
extern int checkH19(void);
extern int checkI0(void);
extern int checkI1(void);
extern int checkI2(void);
extern int checkI3(void);
extern int checkI4(void);
extern int checkI5(void);
extern int checkI6(void);
extern int checkI7(void);
extern int checkI8(void);
extern int checkI9(void);
extern int checkI10(void);
extern int checkI11(void);
extern int checkI12(void);
extern int checkI13(void);
extern int checkI14(void);
extern int checkI15(void);
extern int checkI16(void);
extern int checkI17(void);
extern int checkI18(void);
extern int checkI19(void);
extern int checkJ0(void);
extern int checkJ1(void);
extern int checkJ2(void);
extern int checkJ3(void);
extern int checkJ4(void);
extern int checkJ5(void);
extern int checkJ6(void);
extern int checkJ7(void);
extern int checkJ8(void);
extern int checkJ9(void);
extern int checkJ10(void);
extern int checkJ11(void);
extern int checkJ12(void);
extern int checkJ13(void);
extern int checkJ14(void);
extern int checkJ15(void);
extern int checkJ16(void);
extern int checkJ17(void);
extern int checkJ18(void);
extern int checkJ19(void);
extern int checkK0(void);
extern int checkK1(void);
extern int checkK2(void);
extern int checkK3(void);
extern int checkK4(void);
extern int checkK5(void);
extern int checkK6(void);
extern int checkK7(void);
extern int checkK8(void);
extern int checkK9(void);
extern int checkK10(void);
extern int checkK11(void);
extern int checkK12(void);
extern int checkK13(void);
extern int checkK14(void);
extern int checkK15(void);
extern int checkK16(void);
extern int checkK17(void);
extern int checkK18(void);
extern int checkK19(void);
extern int checkL0(void);
extern int checkL1(void);
extern int checkL2(void);
extern int checkL3(void);
extern int checkL4(void);
extern int checkL5(void);
extern int checkL6(void);
extern int checkL7(void);
extern int checkL8(void);
extern int checkL9(void);
extern int checkL10(void);
extern int checkL11(void);
extern int checkL12(void);
extern int checkL13(void);
extern int checkL14(void);
extern int checkL15(void);
extern int checkL16(void);
extern int checkL17(void);
extern int checkL18(void);
extern int checkL19(void);
extern int checkM0(void);
extern int checkM1(void);
extern int checkM2(void);
extern int checkM3(void);
extern int checkM4(void);
extern int checkM5(void);
extern int checkM6(void);
extern int checkM7(void);
extern int checkM8(void);
extern int checkM9(void);
extern int checkM10(void);
extern int checkM11(void);
extern int checkM12(void);
extern int checkM13(void);
extern int checkM14(void);
extern int checkM15(void);
extern int checkM16(void);
extern int checkM17(void);
extern int checkM18(void);
extern int checkM19(void);
extern int checkN0(void);
extern int checkN1(void);
extern int checkN2(void);
extern int checkN3(void);
extern int checkN4(void);
extern int checkN5(void);
extern int checkN6(void);
extern int checkN7(void);
extern int checkN8(void);
extern int checkN9(void);
extern int checkN10(void);
extern int checkN11(void);
extern int checkN12(void);
extern int checkN13(void);
extern int checkN14(void);
extern int checkN15(void);
extern int checkN16(void);
extern int checkN17(void);
extern int checkN18(void);
extern int checkN19(void);
extern int checkO0(void);
extern int checkO1(void);
extern int checkO2(void);
extern int checkO3(void);
extern int checkO4(void);
extern int checkO5(void);
extern int checkO6(void);
extern int checkO7(void);
extern int checkO8(void);
extern int checkO9(void);
extern int checkO10(void);
extern int checkO11(void);
extern int checkO12(void);
extern int checkO13(void);
extern int checkO14(void);
extern int checkO15(void);
extern int checkO16(void);
extern int checkO17(void);
extern int checkO18(void);
extern int checkO19(void);
extern int checkP0(void);
extern int checkP1(void);
extern int checkP2(void);
extern int checkP3(void);
extern int checkP4(void);
extern int checkP5(void);
extern int checkP6(void);
extern int checkP7(void);
extern int checkP8(void);
extern int checkP9(void);
extern int checkP10(void);
extern int checkP11(void);
extern int checkP12(void);
extern int checkP13(void);
extern int checkP14(void);
extern int checkP15(void);
extern int checkP16(void);
extern int checkP17(void);
extern int checkP18(void);
extern int checkP19(void);
extern int checkQ0(void);
extern int checkQ1(void);
extern int checkQ2(void);
extern int checkQ3(void);
extern int checkQ4(void);
extern int checkQ5(void);
extern int checkQ6(void);
extern int checkQ7(void);
extern int checkQ8(void);
extern int checkQ9(void);
extern int checkQ10(void);
extern int checkQ11(void);
extern int checkQ12(void);
extern int checkQ13(void);
extern int checkQ14(void);
extern int checkQ15(void);
extern int checkQ16(void);
extern int checkQ17(void);
extern int checkQ18(void);
extern int checkQ19(void);
extern int checkR0(void);
extern int checkR1(void);
extern int checkR2(void);
extern int checkR3(void);
extern int checkR4(void);
extern int checkR5(void);
extern int checkR6(void);
extern int checkR7(void);
extern int checkR8(void);
extern int checkR9(void);
extern int checkR10(void);
extern int checkR11(void);
extern int checkR12(void);
extern int checkR13(void);
extern int checkR14(void);
extern int checkR15(void);
extern int checkR16(void);
extern int checkR17(void);
extern int checkR18(void);
extern int checkR19(void);
extern int checkS0(void);
extern int checkS1(void);
extern int checkS2(void);
extern int checkS3(void);
extern int checkS4(void);
extern int checkS5(void);
extern int checkS6(void);
extern int checkS7(void);
extern int checkS8(void);
extern int checkS9(void);
extern int checkS10(void);
extern int checkS11(void);
extern int checkS12(void);
extern int checkS13(void);
extern int checkS14(void);
extern int checkS15(void);
extern int checkS16(void);
extern int checkS17(void);
extern int checkS18(void);
extern int checkS19(void);
extern int checkT0(void);
extern int checkT1(void);
extern int checkT2(void);
extern int checkT3(void);
extern int checkT4(void);
extern int checkT5(void);
extern int checkT6(void);
extern int checkT7(void);
extern int checkT8(void);
extern int checkT9(void);
extern int checkT10(void);
extern int checkT11(void);
extern int checkT12(void);
extern int checkT13(void);
extern int checkT14(void);
extern int checkT15(void);
extern int checkT16(void);
extern int checkT17(void);
extern int checkT18(void);
extern int checkT19(void);
extern int checkU0(void);
extern int checkU1(void);
extern int checkU2(void);
extern int checkU3(void);
extern int checkU4(void);
extern int checkU5(void);
extern int checkU6(void);
extern int checkU7(void);
extern int checkU8(void);
extern int checkU9(void);
extern int checkU10(void);
extern int checkU11(void);
extern int checkU12(void);
extern int checkU13(void);
extern int checkU14(void);
extern int checkU15(void);
extern int checkU16(void);
extern int checkU17(void);
extern int checkU18(void);
extern int checkU19(void);
extern int checkV0(void);
extern int checkV1(void);
extern int checkV2(void);
extern int checkV3(void);
extern int checkV4(void);
extern int checkV5(void);
extern int checkV6(void);
extern int checkV7(void);
extern int checkV8(void);
extern int checkV9(void);
extern int checkV10(void);
extern int checkV11(void);
extern int checkV12(void);
extern int checkV13(void);
extern int checkV14(void);
extern int checkV15(void);
extern int checkV16(void);
extern int checkV17(void);
extern int checkV18(void);
extern int checkV19(void);
extern int checkW0(void);
extern int checkW1(void);
extern int checkW2(void);
extern int checkW3(void);
extern int checkW4(void);
extern int checkW5(void);
extern int checkW6(void);
extern int checkW7(void);
extern int checkW8(void);
extern int checkW9(void);
extern int checkW10(void);
extern int checkW11(void);
extern int checkW12(void);
extern int checkW13(void);
extern int checkW14(void);
extern int checkW15(void);
extern int checkW16(void);
extern int checkW17(void);
extern int checkW18(void);
extern int checkW19(void);
extern int checkX0(void);
extern int checkX1(void);
extern int checkX2(void);
extern int checkX3(void);
extern int checkX4(void);
extern int checkX5(void);
extern int checkX6(void);
extern int checkX7(void);
extern int checkX8(void);
extern int checkX9(void);
extern int checkX10(void);
extern int checkX11(void);
extern int checkX12(void);
extern int checkX13(void);
extern int checkX14(void);
extern int checkX15(void);
extern int checkX16(void);
extern int checkX17(void);
extern int checkX18(void);
extern int checkX19(void);

static const char table_letters[TABLE_COUNT] = "ABCDEFGHIJKLMNOPQRSTUVWX";

static const fn_t ref_fns[TABLE_COUNT] = {
    refA, refB, refC, refD, refE, refF, refG, refH,
    refI, refJ, refK, refL, refM, refN, refO, refP,
    refQ, refR, refS, refT, refU, refV, refW, refX
};

static const fn_t check_fns[TABLE_COUNT][CHECKS_PER_TABLE] = {
    { checkA0, checkA1, checkA2, checkA3, checkA4, checkA5, checkA6, checkA7,
      checkA8, checkA9, checkA10, checkA11, checkA12, checkA13, checkA14, checkA15,
      checkA16, checkA17, checkA18, checkA19 },
    { checkB0, checkB1, checkB2, checkB3, checkB4, checkB5, checkB6, checkB7,
      checkB8, checkB9, checkB10, checkB11, checkB12, checkB13, checkB14, checkB15,
      checkB16, checkB17, checkB18, checkB19 },
    { checkC0, checkC1, checkC2, checkC3, checkC4, checkC5, checkC6, checkC7,
      checkC8, checkC9, checkC10, checkC11, checkC12, checkC13, checkC14, checkC15,
      checkC16, checkC17, checkC18, checkC19 },
    { checkD0, checkD1, checkD2, checkD3, checkD4, checkD5, checkD6, checkD7,
      checkD8, checkD9, checkD10, checkD11, checkD12, checkD13, checkD14, checkD15,
      checkD16, checkD17, checkD18, checkD19 },
    { checkE0, checkE1, checkE2, checkE3, checkE4, checkE5, checkE6, checkE7,
      checkE8, checkE9, checkE10, checkE11, checkE12, checkE13, checkE14, checkE15,
      checkE16, checkE17, checkE18, checkE19 },
    { checkF0, checkF1, checkF2, checkF3, checkF4, checkF5, checkF6, checkF7,
      checkF8, checkF9, checkF10, checkF11, checkF12, checkF13, checkF14, checkF15,
      checkF16, checkF17, checkF18, checkF19 },
    { checkG0, checkG1, checkG2, checkG3, checkG4, checkG5, checkG6, checkG7,
      checkG8, checkG9, checkG10, checkG11, checkG12, checkG13, checkG14, checkG15,
      checkG16, checkG17, checkG18, checkG19 },
    { checkH0, checkH1, checkH2, checkH3, checkH4, checkH5, checkH6, checkH7,
      checkH8, checkH9, checkH10, checkH11, checkH12, checkH13, checkH14, checkH15,
      checkH16, checkH17, checkH18, checkH19 },
    { checkI0, checkI1, checkI2, checkI3, checkI4, checkI5, checkI6, checkI7,
      checkI8, checkI9, checkI10, checkI11, checkI12, checkI13, checkI14, checkI15,
      checkI16, checkI17, checkI18, checkI19 },
    { checkJ0, checkJ1, checkJ2, checkJ3, checkJ4, checkJ5, checkJ6, checkJ7,
      checkJ8, checkJ9, checkJ10, checkJ11, checkJ12, checkJ13, checkJ14, checkJ15,
      checkJ16, checkJ17, checkJ18, checkJ19 },
    { checkK0, checkK1, checkK2, checkK3, checkK4, checkK5, checkK6, checkK7,
      checkK8, checkK9, checkK10, checkK11, checkK12, checkK13, checkK14, checkK15,
      checkK16, checkK17, checkK18, checkK19 },
    { checkL0, checkL1, checkL2, checkL3, checkL4, checkL5, checkL6, checkL7,
      checkL8, checkL9, checkL10, checkL11, checkL12, checkL13, checkL14, checkL15,
      checkL16, checkL17, checkL18, checkL19 },
    { checkM0, checkM1, checkM2, checkM3, checkM4, checkM5, checkM6, checkM7,
      checkM8, checkM9, checkM10, checkM11, checkM12, checkM13, checkM14, checkM15,
      checkM16, checkM17, checkM18, checkM19 },
    { checkN0, checkN1, checkN2, checkN3, checkN4, checkN5, checkN6, checkN7,
      checkN8, checkN9, checkN10, checkN11, checkN12, checkN13, checkN14, checkN15,
      checkN16, checkN17, checkN18, checkN19 },
    { checkO0, checkO1, checkO2, checkO3, checkO4, checkO5, checkO6, checkO7,
      checkO8, checkO9, checkO10, checkO11, checkO12, checkO13, checkO14, checkO15,
      checkO16, checkO17, checkO18, checkO19 },
    { checkP0, checkP1, checkP2, checkP3, checkP4, checkP5, checkP6, checkP7,
      checkP8, checkP9, checkP10, checkP11, checkP12, checkP13, checkP14, checkP15,
      checkP16, checkP17, checkP18, checkP19 },
    { checkQ0, checkQ1, checkQ2, checkQ3, checkQ4, checkQ5, checkQ6, checkQ7,
      checkQ8, checkQ9, checkQ10, checkQ11, checkQ12, checkQ13, checkQ14, checkQ15,
      checkQ16, checkQ17, checkQ18, checkQ19 },
    { checkR0, checkR1, checkR2, checkR3, checkR4, checkR5, checkR6, checkR7,
      checkR8, checkR9, checkR10, checkR11, checkR12, checkR13, checkR14, checkR15,
      checkR16, checkR17, checkR18, checkR19 },
    { checkS0, checkS1, checkS2, checkS3, checkS4, checkS5, checkS6, checkS7,
      checkS8, checkS9, checkS10, checkS11, checkS12, checkS13, checkS14, checkS15,
      checkS16, checkS17, checkS18, checkS19 },
    { checkT0, checkT1, checkT2, checkT3, checkT4, checkT5, checkT6, checkT7,
      checkT8, checkT9, checkT10, checkT11, checkT12, checkT13, checkT14, checkT15,
      checkT16, checkT17, checkT18, checkT19 },
    { checkU0, checkU1, checkU2, checkU3, checkU4, checkU5, checkU6, checkU7,
      checkU8, checkU9, checkU10, checkU11, checkU12, checkU13, checkU14, checkU15,
      checkU16, checkU17, checkU18, checkU19 },
    { checkV0, checkV1, checkV2, checkV3, checkV4, checkV5, checkV6, checkV7,
      checkV8, checkV9, checkV10, checkV11, checkV12, checkV13, checkV14, checkV15,
      checkV16, checkV17, checkV18, checkV19 },
    { checkW0, checkW1, checkW2, checkW3, checkW4, checkW5, checkW6, checkW7,
      checkW8, checkW9, checkW10, checkW11, checkW12, checkW13, checkW14, checkW15,
      checkW16, checkW17, checkW18, checkW19 },
    { checkX0, checkX1, checkX2, checkX3, checkX4, checkX5, checkX6, checkX7,
      checkX8, checkX9, checkX10, checkX11, checkX12, checkX13, checkX14, checkX15,
      checkX16, checkX17, checkX18, checkX19 },
};

extern const unsigned long dataA[128];
extern const unsigned long dataB[128];
extern const unsigned long dataC[128];
extern const unsigned long dataD[128];
extern const unsigned long dataE[128];
extern const unsigned long dataF[128];
extern const unsigned long dataG[128];
extern const unsigned long dataH[128];
extern const unsigned long dataI[128];
extern const unsigned long dataJ[128];
extern const unsigned long dataK[128];
extern const unsigned long dataL[128];
extern const unsigned long dataM[128];
extern const unsigned long dataN[128];
extern const unsigned long dataO[128];
extern const unsigned long dataP[128];
extern const unsigned long dataQ[128];
extern const unsigned long dataR[128];
extern const unsigned long dataS[128];
extern const unsigned long dataT[128];
extern const unsigned long dataU[128];
extern const unsigned long dataV[128];
extern const unsigned long dataW[128];
extern const unsigned long dataX[128];

/* jen pro vypis adresy kritickeho cteni dataX[0x77] */
static const unsigned long *const data_tabs[TABLE_COUNT] = {
    dataA, dataB, dataC, dataD, dataE, dataF, dataG, dataH,
    dataI, dataJ, dataK, dataL, dataM, dataN, dataO, dataP,
    dataQ, dataR, dataS, dataT, dataU, dataV, dataW, dataX
};

#define ITERATIONS        100UL          /* N: pevny pocet cyklu (FINAL.md kap. 4) */
#define DELAY_MS          1000UL         /* pauza mezi cykly */
#define EXPECTED_VALUE    0xFFFFFFFFUL   /* data[0] ^ data[0x77] = 0x00000000 ^ 0xFFFFFFFF */
#define DATA_CRIT_INDEX   0x77           /* data[0x77] */
#define CHECK_CRIT_OFS    0x30UL         /* checkXn + 0x30: ld.w r10,r8[512] (kriticke cteni) */
#define REF_CRIT_OFS      0x04UL         /* refX + 0x04:    ld.w r12,r8[476] (cteni data[0x77]) */

/** statistika jedne bunky (check nebo ref) za cely beh, porovnani s EXPECTED_VALUE */
typedef struct {
    unsigned long count;       /* pocet cyklu s chybnou hodnotou */
    unsigned long first_val;   /* prvni chybna hodnota */
    unsigned long bits;        /* OR (hodnota ^ EXPECTED_VALUE) pres vsechny chyby */
} cell_t;

typedef struct {
    /* pole merici smycky (rozlozeni se nemeni, aby merici smycka zustala stejna):
     * porovnani check s ref, vypisuje se jen jako vs_ref */
    unsigned long err_count[TABLE_COUNT][CHECKS_PER_TABLE];
    unsigned long iterations;
    unsigned long total_mismatches;
    unsigned long iter_mismatches;
    unsigned long min_mismatches;
    unsigned long max_mismatches;
    int ref_val[TABLE_COUNT];
    int check_val[TABLE_COUNT][CHECKS_PER_TABLE];
    /* nezavisle vyhodnoceni: kazde cteni check i ref proti EXPECTED_VALUE */
    cell_t cell[TABLE_COUNT][CHECKS_PER_TABLE];
    cell_t ref_cell[TABLE_COUNT];
    unsigned long iter_errors;        /* chybne check v tomto cyklu */
    unsigned long iter_ref_errors;    /* chybne ref v tomto cyklu */
    unsigned long errors;             /* chybne check za cely beh */
    unsigned long ref_errors;         /* chybne ref za cely beh */
    unsigned long failed_cycles;      /* cykly s >= 1 chybou (check nebo ref) */
    unsigned long first_error_cycle;  /* 0 = zadna chyba */
    unsigned long min_errors;         /* min. chybnych check za cyklus */
    unsigned long max_errors;         /* max. chybnych check za cyklus */
    unsigned long vs_ref_total;       /* soucet iter_mismatches (check != ref, informativni) */
} stats_t;

static void cell_update(cell_t *cell, unsigned long v)
{
    if (cell->count == 0UL) {
        cell->first_val = v;
    }
    cell->count++;
    cell->bits |= v ^ EXPECTED_VALUE;
}

/* '*' = chybna hodnota v tomto cyklu, '.' = ted OK, ale drive chybovala, ' ' = nikdy */
static char cell_symbol(const cell_t *cell, int val)
{
    if ((unsigned long)val != EXPECTED_VALUE) {
        return '*';
    }
    return (cell->count != 0UL) ? '.' : ' ';
}

static void print_status_row(char letter, const cell_t *cell, const int *check_val)
{
    int c;

    TPRINTF("%c:", letter);
    for (c = 0; c < CHECKS_PER_TABLE; c++) {
        TPRINTF(" %c", cell_symbol(&cell[c], check_val[c]));
    }
    TPRINTF("\n");
}

/* jeden symbol za kazdou tabulku A..X */
static void print_ref_row(const stats_t *s)
{
    int t;

    TPRINTF("ref:");
    for (t = 0; t < TABLE_COUNT; t++) {
        TPRINTF(" %c", cell_symbol(&s->ref_cell[t], s->ref_val[t]));
    }
    TPRINTF("\n");
}

static void __attribute__((noinline)) evaluate_cycle(stats_t *s)
{
    int t, c, failed;
    unsigned long v;

    s->iter_errors = 0UL;
    s->iter_ref_errors = 0UL;

    for (t = 0; t < TABLE_COUNT; t++) {
        v = (unsigned long)s->ref_val[t];
        if (v != EXPECTED_VALUE) {
            cell_update(&s->ref_cell[t], v);
            s->iter_ref_errors++;
        }
        for (c = 0; c < CHECKS_PER_TABLE; c++) {
            v = (unsigned long)s->check_val[t][c];
            if (v != EXPECTED_VALUE) {
                cell_update(&s->cell[t][c], v);
                s->iter_errors++;
            }
        }
    }

    failed = (s->iter_errors + s->iter_ref_errors) != 0UL;
    s->errors += s->iter_errors;
    s->ref_errors += s->iter_ref_errors;
    s->vs_ref_total += s->iter_mismatches;
    if (failed) {
        s->failed_cycles++;
        if (s->first_error_cycle == 0UL) {
            s->first_error_cycle = s->iterations;
        }
    }
    if (s->iter_errors < s->min_errors) {
        s->min_errors = s->iter_errors;
    }
    if (s->iter_errors > s->max_errors) {
        s->max_errors = s->iter_errors;
    }

    TPRINTF("%s on iteration=%lu (total failed iterations=%lu)\n",
            failed ? "!!! ERROR !!!" : "PASS", s->iterations, s->failed_cycles);

    for (t = 0; t < TABLE_COUNT; t++) {
        print_status_row(table_letters[t], s->cell[t], s->check_val[t]);
    }
    print_ref_row(s);
}

static void run_measurement(stats_t *s)
{
    int *ref_val = s->ref_val;
    int (*check_val)[CHECKS_PER_TABLE] = s->check_val;
    int t, c, mismatch = 0;

    s->iterations++;
    s->iter_mismatches = 0;

    for (t = 0; t < TABLE_COUNT; t++) {
        ref_val[t] = ref_fns[t]();
    }

    for (t = 0; t < TABLE_COUNT; t++) {
        for (c = 0; c < CHECKS_PER_TABLE; c++) {
            check_val[t][c] = check_fns[t][c]();
            if (check_val[t][c] != ref_val[t]) {
                s->err_count[t][c]++;
                s->iter_mismatches++;
                mismatch = 1;
            }
        }
    }

    s->total_mismatches += (unsigned long)mismatch;

    evaluate_cycle(s);
}

static void print_summary(const stats_t *s)
{
    TPRINTF("Summary iter=%lu tests=%lu errors=%lu ref_errors=%lu vs_ref=%lu\n",
            s->iterations, (unsigned long)TESTS_PER_ITER, s->iter_errors,
            s->iter_ref_errors, s->iter_mismatches);
}

/* 1. cyklus: kazde chybne cteni s tabulkou, sloupcem, adresou a hodnotou */
static void print_cycle_detail(const stats_t *s)
{
    int t, c;
    unsigned long v, adr;

    TPRINTF("Detail iter=%lu wrong=%lu (checks %lu, refs %lu): ERR <table> <cNN|ref> ld=<instr> adr=<data> val=<value>\n",
            s->iterations, s->iter_errors + s->iter_ref_errors, s->iter_errors, s->iter_ref_errors);
    for (t = 0; t < TABLE_COUNT; t++) {
        adr = (unsigned long)&data_tabs[t][DATA_CRIT_INDEX];
        v = (unsigned long)s->ref_val[t];
        if (v != EXPECTED_VALUE) {
            TPRINTF("ERR %c ref ld=%08lx adr=%08lx val=%08lx\n", table_letters[t],
                    (unsigned long)ref_fns[t] + REF_CRIT_OFS, adr, v);
        }
        for (c = 0; c < CHECKS_PER_TABLE; c++) {
            v = (unsigned long)s->check_val[t][c];
            if (v != EXPECTED_VALUE) {
                TPRINTF("ERR %c c%02d ld=%08lx adr=%08lx val=%08lx\n", table_letters[t], c,
                        (unsigned long)check_fns[t][c] + CHECK_CRIT_OFS, adr, v);
            }
        }
    }
}

/* zaverecny souhrn po bunkach: jen bunky s alespon jednou chybou */
static void print_cells(const stats_t *s)
{
    int t, c;
    unsigned long n_check = 0UL, n_ref = 0UL;
    const cell_t *p;

    for (t = 0; t < TABLE_COUNT; t++) {
        n_ref += (s->ref_cell[t].count != 0UL);
        for (c = 0; c < CHECKS_PER_TABLE; c++) {
            n_check += (s->cell[t][c].count != 0UL);
        }
    }

    TPRINTF("Cells with errors: checks %lu/%lu, refs %lu/%lu: CELL <table> <cNN|ref> n=<cycles> first=<value> bits=<OR of value^%08lx>\n",
            n_check, (unsigned long)TESTS_PER_ITER, n_ref, (unsigned long)TABLE_COUNT, EXPECTED_VALUE);
    for (t = 0; t < TABLE_COUNT; t++) {
        p = &s->ref_cell[t];
        if (p->count != 0UL) {
            TPRINTF("CELL %c ref n=%lu first=%08lx bits=%08lx\n", table_letters[t], p->count, p->first_val, p->bits);
        }
        for (c = 0; c < CHECKS_PER_TABLE; c++) {
            p = &s->cell[t][c];
            if (p->count != 0UL) {
                TPRINTF("CELL %c c%02d n=%lu first=%08lx bits=%08lx\n", table_letters[t], c, p->count, p->first_val, p->bits);
            }
        }
    }
}

static void print_plan(void)
{
    TPRINTF("ID plan cycles=%lu checks_per_cycle=%lu refs_per_cycle=%lu expected=0x%08lx delay_ms=%lu ref_row=A..X\n",
            ITERATIONS, (unsigned long)TESTS_PER_ITER, (unsigned long)TABLE_COUNT, EXPECTED_VALUE, DELAY_MS);
}

static void print_final_summary(const stats_t *s)
{
    unsigned long checks = s->iterations * (unsigned long)TESTS_PER_ITER;
    unsigned long refs = s->iterations * (unsigned long)TABLE_COUNT;

    TPRINTF("Summary  = cycles:%lu/%lu, checks/cycle:%lu, checks:%lu, errors:%lu, failed cycles:%lu, first error cycle:%lu, ",
            s->iterations, ITERATIONS, (unsigned long)TESTS_PER_ITER, checks, s->errors,
            s->failed_cycles, s->first_error_cycle);
    TPRINTF("min err/cycle:%lu, max err/cycle:%lu, ref reads:%lu, ref errors:%lu, vs ref:%lu\n",
            s->min_errors, s->max_errors, refs, s->ref_errors, s->vs_ref_total);
    TPRINTF("Result   = %s (%lu errors in %lu checks, %lu ref errors in %lu ref reads)\n",
            (s->errors + s->ref_errors == 0UL) ? "*** PASS ***" : "!!! FAIL !!!",
            s->errors, checks, s->ref_errors, refs);
}

/* radek RESULT (FINAL.md kap. 4), delsi nez buffer __debug, proto se sklada zde */
static char result_line[384];
static int result_len;

static void result_add(const char *format, ...)
{
    va_list args;
    int n;

    if (result_len >= (int)sizeof(result_line) - 1) {
        return;
    }
    va_start(args, format);
    n = vsnprintf(result_line + result_len, sizeof(result_line) - result_len, format, args);
    va_end(args);
    if (n > 0) {
        result_len += n;
        if (result_len > (int)sizeof(result_line) - 1) {
            result_len = (int)sizeof(result_line) - 1;
        }
    }
}

static void print_result_line(const stats_t *s, const id_info_t *a, const id_info_t *b, const boot_info_t *bi)
{
    char sn[2 * CHIP_SN_SIZE + 1], board[2 * BOARD_ID_SIZE + 1];
    const char *verdict;
    unsigned long crc;
    int i, n;

    if (s->errors + s->ref_errors != 0UL) {
        verdict = "FAIL";
    } else if (s->iterations == ITERATIONS) {
        verdict = "PASS";
    } else {
        verdict = "INCOMPLETE";
    }

    id_hex_str(sn, a->chip_sn, CHIP_SN_SIZE);
    id_hex_str(board, a->board_id, (a->board_len != 0) ? BOARD_ID_SIZE : 0);

    result_len = 0;
    result_add("RESULT;v=1;fw=%s-%s;fw_crc=%08lx;fw_crc_end=%08lx;sn=%s;did=%08lx;board=%s",
               FW_NAME, FW_VER, a->image_crc, b->image_crc, sn, a->did,
               (a->board_len != 0) ? board : "none");
    result_add(";gpf=%08lx;fcr=%08lx;rcause=%08lx;boot_tmo=%lu;N=%lu;cycles=%lu;checks=%lu",
               a->gpf_lo, a->fcr, bi->rcause, bi->timeouts, ITERATIONS, s->iterations,
               s->iterations * (unsigned long)TESTS_PER_ITER);
    result_add(";errors=%lu;failcyc=%lu;first=%lu;min=%lu;max=%lu;ref=%lu;verdict=%s",
               s->errors, s->failed_cycles, s->first_error_cycle, s->min_errors, s->max_errors,
               s->ref_errors, verdict);
    /* crc = CRC32 (zlib) vseho pred ";crc=" */
    crc = crc32_calc((const volatile unsigned char *)result_line, (unsigned long)result_len);
    result_add(";crc=%08lx", crc);

    for (i = 0; i < result_len; i += n) {
        n = result_len - i;
        if (n > 128) {
            n = 128;
        }
        TPRINTF("%.*s", n, result_line + i);
    }
    TPRINTF("\n");
}

static unsigned long read_chip_id(void)
{
    return (unsigned long)__builtin_mfdr(AVR32_DID);
}

#define CHIP_PN_UC3A0512  0x1EDCUL

static void print_chip_id(void)
{
    unsigned long id = read_chip_id();
    unsigned long rn = (id & AVR32_DID_RN_MASK) >> AVR32_DID_RN_OFFSET;
    unsigned long pn = (id & AVR32_DID_PN_MASK) >> AVR32_DID_PN_OFFSET;
    unsigned long mid = (id & AVR32_DID_MID_MASK) >> AVR32_DID_MID_OFFSET;

    TPRINTF("Chip ID  = 0x%08lx (rev %c, ", id, (char)('A' + rn));
    if (pn == CHIP_PN_UC3A0512) {
        TPRINTF("AT32UC3A0512, ");
    } else {
        TPRINTF("unknown PN 0x%04lx, ", pn);
    }
    if (mid == AVR32_DID_MID_ATMEL) {
        TPRINTF("Atmel)\n");
    } else {
        TPRINTF("unknown MID 0x%03lx)\n", mid);
    }
}

#define SERIAL_NUMBER_ADDRESS  0x80800204UL
#define SERIAL_NUMBER_SIZE     15

static void print_serial_number(void)
{
    const volatile unsigned char *sn = (const volatile unsigned char *)SERIAL_NUMBER_ADDRESS;
    int i;

    TPRINTF("Chip SN  = ");
    for (i = 0; i < SERIAL_NUMBER_SIZE; i++) {
        TPRINTF("%02x", (unsigned int)sn[i]);
    }
    TPRINTF("\n");
}

int main(void)
{
    static stats_t stats;
    static boot_info_t boot;
    static id_info_t id_start, id_end;

    /* hodiny RC -> OSC0 -> PLL0 se znackami na UART a timeouty (lowleveinit.c) */
    boot_clock_init(&boot);

    stats.min_errors = (unsigned long)TESTS_PER_ITER;

    TPRINTF("=== START ===\n");
    print_chip_id();
    print_serial_number();
    spi_flash_init();
    spi_flash_print_serial_number();
    id_collect(&id_start);
    id_print(&id_start, &boot);
    print_plan();
    TPRINTF("\n");

    while (stats.iterations < ITERATIONS) {
        run_measurement(&stats);
        print_summary(&stats);
        if (stats.iterations == 1UL && (stats.iter_errors + stats.iter_ref_errors) != 0UL) {
            print_cycle_detail(&stats);
        }

        TEST_DELAY_MS(DELAY_MS);
    }

    TPRINTF("=== FINISH ===\n");
    print_chip_id();
    print_serial_number();
    spi_flash_print_serial_number();
    id_collect(&id_end);
    id_print(&id_end, &boot);
    print_plan();
    print_cells(&stats);
    print_final_summary(&stats);
    print_result_line(&stats, &id_start, &id_end, &boot);

    for (;;) {
    }

    return 0;
}
