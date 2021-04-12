
/*
CCU40 CC40 Timer
CCU40 CC41 级联CC40 
CCU40 CC42 Counter
*/

#include <XMC4400.h>
#include "CCU4.h"

#define CCU40_CC40_DIV_VAL	12
#define CCU40_CC40_PRS_VAL	14648    //Timer  (1/120 000 000)*(2^DIV_VAL)*(PRS_VAL+1) s    ~0.5s

#define CCU40_CC41_DIV_VAL	12
#define CCU40_CC41_PRS_VAL	2        //Contatenation ~0.5s*(PRS_VAL+1) 1.5s 进一次周期中断  
                                     //xdm有没有注意到，这样一来就可以实现32位定时了，分频可以低一点，定时值大一点，定时更加精准

#define CCU40_CC42_DIV_VAL  4
#define CCU40_CC42_PRS_VAL  60       //Counter

void CCU4_Init(void)
{
    WR_REG(SCU_RESET->PRSET0, SCU_RESET_PRSET0_CCU40RS_Msk, SCU_RESET_PRSET0_CCU40RS_Pos, 1);
	WR_REG(SCU_RESET->PRCLR0, SCU_RESET_PRCLR0_CCU40RS_Msk, SCU_RESET_PRCLR0_CCU40RS_Pos, 1);	
	WR_REG(SCU_CLK->CLKSET, SCU_CLK_CLKSET_CCUCEN_Msk, SCU_CLK_CLKSET_CCUCEN_Pos, 1);	
	
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_SPRB_Msk, CCU4_GIDLC_SPRB_Pos, 1);
	WR_REG(CCU40_CC40->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, CCU40_CC40_DIV_VAL);  //分频
	WR_REG(CCU40_CC40->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, CCU40_CC40_PRS_VAL);    //周期值
	WR_REG(CCU40->GCSS, CCU4_GCSS_S0SE_Msk, CCU4_GCSS_S0SE_Pos, 1);                             //传送周期值
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS0I_Msk, CCU4_GIDLC_CS0I_Pos, 1);	                        //启用定时器片

	WR_REG(CCU40_CC41->CMC, CCU4_CC4_CMC_TCE_Msk, CCU4_CC4_CMC_TCE_Pos, 1);                     //CC41-40级联
	WR_REG(CCU40_CC41->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, CCU40_CC41_DIV_VAL);  
	WR_REG(CCU40_CC41->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, CCU40_CC41_PRS_VAL);    
	WR_REG(CCU40->GCSS, CCU4_GCSS_S1SE_Msk, CCU4_GCSS_S1SE_Pos, 1);                             
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS1I_Msk, CCU4_GIDLC_CS1I_Pos, 1);	                        
	WR_REG(CCU40_CC41->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);
	WR_REG(CCU40_CC41->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 0);                   //周期匹配中断至SR0	
	
	WR_REG(CCU40_CC42->PSC, CCU4_CC4_PSC_PSIV_Msk, CCU4_CC4_PSC_PSIV_Pos, CCU40_CC42_DIV_VAL);  //外部计数时，这个分频影响采样率
	WR_REG(CCU40_CC42->INS, CCU4_CC4_INS_EV0IS_Msk, CCU4_CC4_INS_EV0IS_Pos, 0);                 //事件0 P1.1
	WR_REG(CCU40_CC42->INS, CCU4_CC4_INS_EV0EM_Msk, CCU4_CC4_INS_EV0EM_Pos, 1);                 //上升沿有效
	WR_REG(CCU40_CC42->CMC, CCU4_CC4_CMC_CNTS_Msk, CCU4_CC4_CMC_CNTS_Pos, 1);                   //事件0：计数
	WR_REG(CCU40_CC42->PRS, CCU4_CC4_PRS_PRS_Msk, CCU4_CC4_PRS_PRS_Pos, CCU40_CC42_PRS_VAL); 
	WR_REG(CCU40->GCSS, CCU4_GCSS_S2SE_Msk, CCU4_GCSS_S2SE_Pos, 1);              
	WR_REG(CCU40->GIDLC, CCU4_GIDLC_CS2I_Msk, CCU4_GIDLC_CS2I_Pos, 1);	       
	WR_REG(CCU40_CC42->INTE, CCU4_CC4_INTE_PME_Msk, CCU4_CC4_INTE_PME_Pos, 1);		
	WR_REG(CCU40_CC42->SRS, CCU4_CC4_SRS_POSR_Msk, CCU4_CC4_SRS_POSR_Pos, 1);                   //周期匹配中断SR1
}

void CCU40_CC40_Start()
{
	WR_REG(CCU40_CC40->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);//启动CCU40_CC40
}

void CCU40_CC41_Start()
{
	WR_REG(CCU40_CC41->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);//启动CCU40_CC40
}

void CCU40_CC42_Start()
{
	WR_REG(CCU40_CC42->TCSET, CCU4_CC4_TCSET_TRBS_Msk, CCU4_CC4_TCSET_TRBS_Pos, 1);//启动CCU40_CC40
}
