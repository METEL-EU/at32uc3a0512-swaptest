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

#define REF_FUNC(letter) \
int ref##letter(void) \
{ \
    unsigned long s1, t1, t2; \
    s1 = 0x00000077UL; \
    { \
        t1 = \
            data##letter[0] ^ \
            data##letter[0] ^ \
            data##letter[0]; \
        t2 = \
            data##letter[0] ^ \
            data##letter[s1 & 255]; \
    } \
    return (int)t2; \
}

REF_FUNC(A)
REF_FUNC(B)
REF_FUNC(C)
REF_FUNC(D)
REF_FUNC(E)
REF_FUNC(F)
REF_FUNC(G)
REF_FUNC(H)
REF_FUNC(I)
REF_FUNC(J)
REF_FUNC(K)
REF_FUNC(L)
REF_FUNC(M)
REF_FUNC(N)
REF_FUNC(O)
REF_FUNC(P)
REF_FUNC(Q)
REF_FUNC(R)
REF_FUNC(S)
REF_FUNC(T)
REF_FUNC(U)
REF_FUNC(V)
REF_FUNC(W)
REF_FUNC(X)
