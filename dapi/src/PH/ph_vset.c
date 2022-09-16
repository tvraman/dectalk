
/* 
 ***********************************************************************
 *
 *                           Coryright (c)
 *    � Digital Equipment Corporation 1996, 1997, 1998. All rights reserved.
 *
 *    Restricted Rights: Use, duplication, or disclosure by the U.S.
 *    Government is subject to restrictions as set forth in subparagraph
 *    (c) (1) (ii) of DFARS 252.227-7013, or in FAR 52.227-19, or in FAR
 *    52.227-14 Alt. III, as applicable.
 *
 *    This software is proprietary to and embodies the confidential
 *    technology of Digital Equipment Corporation and other parties.
 *    Possession, use, or copying of this software and media is authorized
 *    only pursuant to a valid written license from Digital or an
 *    authorized sublicensor.
 *
 ***********************************************************************
 *    File Name:    ph_vset.c
 *    Author:       Dennis Klatt
 *    Creation Date:01-JAN-1985
 *
 *    Functionality:
 * 	  This file contains functions
 * 	  to select new speakers, to change individual
 * 	  fields of a speaker definition, and to
 * 	  convert the values from the format used by
 * 	  the voice commands to the format used by
 * 	  the signal processor. Based on the one for
 * 	  the new synthesizer.
 *
 ***********************************************************************
 *    Revision History:
 *
 * Rev  Who     Date        Description
 * ---  -----   ----------- -------------------------------------------- 
 * 0001 DK		01/01/1985  Initial creation
 * 0002 DGC		01/02/1985  The speaker def. code called "SP_AS" "2".
 *          				Not a bug, just a tidy up.
 * 0003 DGC 	01/19/1985  The LIMIT structure def. is now in phdefs.h
 * 0004 DK		02/19/1985  Send to chip only pars that are used
#ifdef ENGLISH_US           Merged from phvset.c
 * 0005 DK		01/13/1985  Add Vax-only code to print spdefs for manual
 * 0006 DGC		06/18/1985  "g5" => "lo".
 * 0007 DK		07/18/1985  Rescale "SP_FT".
 * 0008 DK		07/25/1985  Rescale spdeftltoff.
 * 0009 MGS		03/25/1996  Merged Win95 Code with 42c
 * 0010 EAB		04/04/1996  SP_BR has had a bug since day one it wanted to be
 *                      	sp_br**2 >>1 not <<1 so that 100(max) is close to
 *                      	096(1 in frac arith) + sp_br**2 >>1 aprox max
 *                      	value of 8192.
 * 0011 tek		04/23/1996  remove spurious printf from 0010 (v26)
 * 0012 MGS		05/29/1996 	merged 0010 ad 0011 from english
#endif
#ifdef SPANISH              Merged from sphvset.c
 * 0005 MM		04/02/1985  68000
 * 0006 DF		03/06/1986  DT3 updates:  SP_EF -> SP_LX ; SP_G5 -> SP_LO
#endif
 * 0011	MGS		06/04/1996 	Merged spanish with english
 * 0012	MGS		06/06/1996	Changed file name from phvset.c ph_vset.c
 * 0013	MGS		06/18/1996	Merged German with english/spanish
 * 0014	GL		04/21/1997	BATS#360  remove spaces before "#define" or "#if" 
 * 0015 EAB		05/07/1997	EAB Adjust overall imbalance of apiration
 * 0016 DR		09/30/1997	UK BUILD: added UK STUFF
 * 0017 MGS     04/06/1998  made curspdef contain the speaker number
 * 0018 JAW     04/27/1998  Changed setparam so it will accept parameters up to SP_GS
 *                          instead of SP_SR.
 * 0020 EAB		04/27/98	Added code to support gs speaker def tha JAW added to arrray
 * 0021 JAW     05/04/1998  Made code from 4/6/98 only be compiled under non-MS-DOS
 *                          environments.
 * 0022 EAB     7/14/98		Move code from malboundary to locus theory for SPANISH
 * 0023 EAB		10/9/98		Verified hypothesis that malamp and femamp tables can be the same
							and implemented same saves some space and lots of tuning
 * 0024 EAB		10/15/98	Removed German fudge factors code "more better"
 * 0025	EAB		10/16/98	Modifiy the code to make Castillion more distinct from latin by making the fric
							and aspir stronger
 * 0026 EAB		10/16/98	Last fix had a bug fixed it
 * 0027 EAB		10/26/98	Reduce scaling of laryngealization most of range was perceptually useless 
 * 0028 EAB		10/24/98	Code to use just mal_amp has proven sucessful-It simplifies tuning and takes advantage
							of the fact that the male voiecs have recieved more attention
 * 0029 EAB		10/29/98	Added code to compensate fo LOWCOMPUTE voice gain changes
 * 0030 EAB		11/16/98	BATS 804 added code to check limits of speaker definition
 * 0031 EAB		6/26/99		Added in ifdef NEW_VOLUME code to test out and document an ph only software volume cobtrol
							this is not implemented 
 * 0032 EAB		10/7/99		remove NEW_VOLUME and replace with Lockheed Martin SW_VOLUME
 * 0033 EAB		10/28/99	Modified to support Lookheed-Martin chnages with ifdef SW_VOLUME
 * 0034 NAL		01/24/00	Modified setparam to ignore tuned params if bDoTuning flag is set
 * 0035 CAB     08/14/200   Removed WINprintf for windows ce.
 */
  
#include "ph_def.h"  
#ifdef WIN32
#include "vtminst.h"	// NAL needed to control tuning
#endif
  
/***************************************************************************/
/* MVP: The following extern variables are now become elements of instance */
/* specific PH thread data structure DPH_T.                                */
/* extern  short   spdefb1off;                                             */
/* extern  short   spdeflaxprcnt;                                          */
/* extern  short   spdeftltoff;                                            */
/* extern  short   f0_dep_tilt;             How much f0 changes tilt.      */
/* extern  short   malfem;                  MALE or FEMALE.                */
/* extern  short   *p_locus;                Locus table.                   */
/* extern  short   *p_diph;                 Dipthong table.                */
/* extern  short   *p_tar;                  Targets.                       */
/* extern  short   *p_amp;                  Amplitudes.                    */
/* extern  short   arg1;                    For "mulsh" et al.             */
/* extern  short   arg2;                                                   */
/* extern  short   arg3;                                                   */
/* extern  short   curspdef[];              Holds current speaker def.     */
/* extern  FLAG    loadspdef;               Flag: need to load TMS320.     */
/* extern  short   *voidef[];               Speaker definition directory   */
/* extern  short   var_val[SPDEF];                                         */
/* extern  short   *voidef_8[];  Speaker definition directory   for 8 kHz. */
/* extern  short   assertiveness;           How much f0 falls at end.      */
/* extern  short   f0_lp_filter;            Cuttoff, f0 lowpass filter.    */
/* extern  short   size_hat_rise;           F0 hat rise, HZ times 10.      */
/* extern  short   scale_str_rise;          F0 scale factor, stress imp.   */
/* extern  short   f0basefall;                                             */
/* extern  short   f0minimum;                                              */
/* extern  short   f0scalefac;                                             */
/***************************************************************************/

/* extern  short   errors;  *//* Error masks.                 */

/* extern  short   last_voice; *//* MVP MI *//* reload voice */


extern short            maleloc[];	   /* Male tables.                 */
extern short            maldip[];
extern short            maltar[];
extern short            malamp[];

extern short            femloc[];	   /* Female tables.               */
extern short            femdip[];
extern short            femtar[];
extern short            femamp[];


/*
 *      Function Name: setparam()       
 *
 *  	Description: This function is called by
 * 					 "kltask" in ph_task.c to zap a parameter in the current
 *					 speaker definition. The "which" is the "SP_"
 *					 symbol; the value is the value. Wild "which"
 *					 values are ignored. Wild values are pulled
 *					 into range, but log an error too.
 *
 *      Arguments: PDPH_T pDph_t,  Pointer to PH thread 
 *								   instance specific data structure
 *				   int which,
 *				   int value
 *
 *      Return Value: void
 *
 *      Comments:
 *
 */
 
void setparam (LPTTS_HANDLE_T phTTS, int which, int value)
{
	register LIMIT         *lp;
	extern   LIMIT         limit[];


	PKSD_T                  pKsd_t = phTTS->pKernelShareData;
	PDPH_T                  pDph_t = phTTS->pPHThreadData;
#ifdef WIN32
	PVTM_T			pVtm_t	=	phTTS->pVTMThreadData;
#endif
	

	int voice = pKsd_t->last_voice;



	if (which < SP_SEX || which > SP_GS) /* changed upper limit from SP_SR to SP_GS, JAW
		                                    4/27/98 */
	{								   /* Check for junk. */
		return;
	}
	lp = &limit[which];

#ifdef WIN32
	if(pVtm_t->bDoTuning) /* Ignore autotuned vals when autotuning *///
	{

		if (value < lp->l_min)
		{								   /* Lower limit check.   */
			value = lp->l_min;
		}
		else if (value > lp->l_max)
		{								   /* Upper limit check.   */
			value = lp->l_max;
		}
		pDph_t->curspdef[which] = value;   /* Zap the value and    */

	}

	else
#endif
	{
		if (value + (pDph_t->tunedef[voice][which]) < lp->l_min)
		{								   /* Lower limit check.   */
			value = lp->l_min;
		}
		else if (value + (pDph_t->tunedef[voice][which]) > lp->l_max)
		{								   /* Upper limit check.   */
			value = lp->l_max;
		}
		pDph_t->curspdef[which] = value + (pDph_t->tunedef[voice][which]) ;   /* Zap the value and    */
	}

	pDph_t->loadspdef = TRUE;		   /* ask for reload.      */
}

/*
 *      Function Name: usevoice()       
 *
 *  	Description: Select a new voice. The
 * 					 "voice" is a voice number, like
 *					 "PERFECT_PAUL". No need to range check
 *					 the number, because there is no ":vo",
 *					 only ":n?", and you cannot get a bad
 *					 voice, unless there is a bad bug.
 *
 *      Arguments: LPTTS_HANDLE_T phTTS, 
 *				   int voice
 *
 *      Return Value: void
 *
 *      Comments:
 *
 */

void usevoice (LPTTS_HANDLE_T phTTS, int voice)
{
	short			*newspdef;
	short			*tunespdef;
	int            i;
	PKSD_T                  pKsd_t = phTTS->pKernelShareData;
	PDPH_T                  pDph_t = phTTS->pPHThreadData;

#ifdef PH_DEBUG
	//eab 11/16/98 BATS 804 
	extern   LIMIT         limit[];
	LIMIT         *lp;
#endif
#ifdef PH_DEBUG
	short value;
#endif
#ifdef MSDOS
	newspdef = pDph_t->voidef[voice];  /* Copy into place.     */
#else
	if (pKsd_t->uiSampleRate < 8763)
	{
		newspdef = pDph_t->voidef_8[voice];
		tunespdef = pDph_t->tunedef_8[voice];
	}
	else
	{
		newspdef = pDph_t->voidef[voice];
		tunespdef = pDph_t->tunedef[voice];
	}
#endif

	for (i = 0; i < SPDEF; ++i)
	{
		pDph_t->curspdef[i] = newspdef[i] + tunespdef[i];
		//eab 11/16/98 BATS 804 If ANY vlaues are out of allowable ranges scream LOUDLY 
#ifdef PH_DEBUG
		value=0;
		lp = &limit[i];
		if (pDph_t->curspdef[i] < lp->l_min)
		{								   /* Lower limit check.   */
			value = lp->l_min;
		}
		else if (pDph_t->curspdef[i] > lp->l_max)
		{								   /* Upper limit check.   */
			value = lp->l_max;
		}
		if(value !=0)
		{
#ifndef UNDER_CE
		WINprintf(" DANGER Speaker definition %d contains the illegal value %d \n",i,pDph_t->curspdef[i]); // CAB 08/14/2000
#endif
		}
#endif
	}

#ifndef MSDOS
	/* MGS made curspdef contain the speaker number */
	pDph_t->curspdef[SP_NM]=voice;
#endif
	pDph_t->loadspdef = TRUE;		   /* Ask for reload.      */
	pKsd_t->last_voice = voice;
}


/*
 *      Function Name: saveval()        
 *
 *  	Description: Make "var_val" the same as
 * 					 the current speaker. Called from the
 * 					 main loop in "phmain" on a SAVE.
 *
 *      Arguments: PDPH_T pDph_t
 *
 *      Return Value: void
 *
 *      Comments:
 *
 */
void saveval (PDPH_T pDph_t)
{
	register int            i;

	for (i = 0; i < SPDEF; ++i)
		pDph_t->var_val[i] = pDph_t->curspdef[i];
}


/*
 *      Function Name: setspdef()        
 *
 *  	Description: This routine is called by
 * 					 "phclause" to recompute and reload a speaker
 * 					 definition. The magic numbers are pulled from the
 *					 "curspdef" array (where they are stored in a form that
 *					 is similar to the user format), converted to the bits
 *					 needed by the signal processor, and blasted over
 *					 to the TMS32010. 
 *
 *      Arguments: LPTTS_HANDLE_T phTTS
 *
 *      Return Value: void
 *
 *      Comments: This routine does not appear to do
 *				  all the stuff that "fcheck" and "convchip" did.
 *				  Is this intentional?
 *
 */

void setspdef (LPTTS_HANDLE_T phTTS)
{
	register S32            nlong;
	SP_CHIP far            *spdef;

	PKSD_T                  pKsd_t = phTTS->pKernelShareData;
	PDPH_T                  pDph_t = phTTS->pPHThreadData;


#ifdef SEPARATE_PROCESSES
	spdef = (SP_CHIP far *) malloc (SPDEF_PARS * sizeof (short));

#else
	spdef = (SP_CHIP far *) spcget (SPC_type_speaker);
#endif
	pDph_t->malfem = pDph_t->curspdef[SP_SEX];	/* Determine SEX   */

	//eab maltars and femtars can be the same (saves tuning) 10/98
	if (pDph_t->malfem == MALE)
	{
		pDph_t->p_locus = maleloc;	   			/* Use MALE tables */
		pDph_t->p_diph = maldip;
		pDph_t->p_tar = maltar;
		pDph_t->p_amp = malamp;


	}
	else
	{
		pDph_t->p_locus = femloc;	   			/* Use FEMALE tables */
		pDph_t->p_diph = femdip;
		pDph_t->p_tar = femtar;
		pDph_t->p_amp = malamp;
	}
#ifdef Old_malamp_and_femamp

	if (pDph_t->malfem == MALE)
	{
		pDph_t->p_locus = maleloc;	   			/* Use MALE tables */
		pDph_t->p_diph = maldip;
		pDph_t->p_tar = maltar;
		pDph_t->p_amp = malamp;


	}
	else
	{
		pDph_t->p_locus = femloc;	   			/* Use FEMALE tables */
		pDph_t->p_diph = femdip;
		pDph_t->p_tar = femtar;
		pDph_t->p_amp = femamp;
	}
#endif
	/* The following are not sent to chip, just used by higher level routines */

	pDph_t->f0_dep_tilt = pDph_t->curspdef[SP_FT];			/* FT -> FT */ /* FL in spanish */
	pDph_t->assertiveness = pDph_t->curspdef[SP_AS] * 41;	/* AS in % -> AS */
/* BATS 711 MOve German to the new method for filter calculation*/
	pDph_t->f0_lp_filter = (1500 + 15 * pDph_t->curspdef[SP_QU]);	/* QU in % -> lp cuttoff */
	pDph_t->size_hat_rise = pDph_t->curspdef[SP_HR] * 10;	/* HR in Hz -> Hz*10    */
	pDph_t->scale_str_rise = pDph_t->curspdef[SP_SR];		/* SR in Hz -> mult. sc. fac */
	pDph_t->f0minimum = pDph_t->curspdef[SP_AP] * 10;		/* AP -> f0min          */
	pDph_t->f0scalefac = pDph_t->curspdef[SP_PR] * 41;		/* PR -> f0scale        */
	pDph_t->f0basefall = pDph_t->curspdef[SP_BF] * 10;		/* baseline fall        */
	pDph_t->spdeflaxprcnt = pDph_t->curspdef[SP_LX] * 41;	/* degree of lax breathiness */
	pDph_t->spdeftltoff = (pDph_t->curspdef[SP_SM] * 25) / 100;		/* SM (%) -> dB tilt    */
	/* printf("WA?br  %d \n",pDph_t->curspdef[SP_BR]); */
	pDph_t->spdefb1off = (pDph_t->curspdef[SP_BR] * pDph_t->curspdef[SP_BR]) >> 1;
	pDph_t->spdefb1off += 4096;		   						/* BR -> B1-offset      */
	/* BR=55 -> scale B1 by 2.5 factor      */
	/* BR= 0 -> scale B1 by 1.0 factor      */
	pDph_t->spdefglspeed = pDph_t->curspdef[SP_GS];
			if(pDph_t->spdefglspeed)
			{
				if(pDph_t->spdefglspeed > 95)
				{
					pDph_t->spdefglspeed = -(95 - pDph_t->spdefglspeed);
				}
				else
				{
				pDph_t->spdefglspeed = ((95-pDph_t->spdefglspeed)*30);
				}
			}
	
	
	
	
	/* 
	 * The following are all sent to the synthesizer chip 
	 */

	spdef->fnscale = (200 - pDph_t->curspdef[SP_HS]) * 41;	/* HS -> FNscale        */
	if (pDph_t->curspdef[SP_F4] == ZAPF)
	{
		spdef->r4cb = ZAPF;
	}
	else
	{
		nlong = pDph_t->curspdef[SP_F4];
		nlong = nlong * spdef->fnscale;
		spdef->r4cb = nlong >> 12;	   		/* F4 -> F4*FNscale     */
	}

	spdef->r4cc = pDph_t->curspdef[SP_B4];	/* B4 -> B4             */
	if (spdef->r4cb > 4950)
	{
		spdef->r4cb = ZAPF;			   		/* F4 too big, zap it   */
		spdef->r4cc = ZAPB;			   		/* and bw   			*/
	}

	if (pDph_t->curspdef[SP_F5] == ZAPF)
	{
		spdef->r5cb = ZAPF;
	}
	else
	{
		nlong = pDph_t->curspdef[SP_F5];
		nlong = nlong * spdef->fnscale;
		spdef->r5cb = nlong >> 12;	   		/* F5 -> F5*FNscale     */
	}

	/* 
	 *  B5 -> B5
	 */

	/*EAB- 6/25/99 This was a test for a new software volume control requested by Tom Kopec for
	a customer. Since the system is tuned "hot" the volume is at a max and these controls allow
	it to be reduced. It works by reducing the the gain of the final cascade resonator (different depending
	on what version of the vocal tract is used), and the two other noise source gains GH and GF. You simply need do 
	decrease these all by the same db amount and the results tract with good fidelity */



	spdef->r5cc = pDph_t->curspdef[SP_B5];
	if (spdef->r5cb > 4950)
	{
		spdef->r5cb = ZAPF;			   		/* F5 too big, zap it   */
		spdef->r5cc = ZAPB;			   		/* and bw   			*/
	}
	spdef->r4pb = pDph_t->curspdef[SP_P4];	/* F7 -> F4p            */
	spdef->r5pb = pDph_t->curspdef[SP_P5];	/* F8 -> F5p            */
	spdef->t0jit = pDph_t->curspdef[SP_LA]; //* 8;		/* LA -> LA/4096 of T0  */
	spdef->r5ca = pDph_t->curspdef[SP_G1];	/* G1 -> G1 in dB       */
	spdef->r4ca = pDph_t->curspdef[SP_G2];	/* G2 -> G2 in dB       */
	spdef->r3ca = pDph_t->curspdef[SP_G3];	/* G3 -> G3 in dB       */
#ifdef NEW_VOLUME
	//Assumes vocal tract with the second formant as the last resonatorr f5-f1-f4-f3-f2 ordering
	spdef->r2ca = pDph_t->curspdef[SP_G4]-temp;	/* G4 -> G4 in dB       */
#else
	spdef->r2ca = pDph_t->curspdef[SP_G4];	/* G4 -> G4 in dB       */
#endif

	spdef->r1ca = pDph_t->curspdef[SP_LO];	/* LO -> LO in dB       */
	/* EAB 12/16/97 Comments incorrect - note comment in ph_vset 
	is not correct this code allows a range of approx 12.5% to 60%  
	for nopen1 as a percentage and not	0 to 100 */
	spdef->nopen1 = 4000 + (160 * (100 - pDph_t->curspdef[SP_RI]));	/* RI -> K1  */
	spdef->nopen2 = pDph_t->curspdef[SP_NF] * 4;/* NF -> K2     */
	spdef->aturb = pDph_t->curspdef[SP_BR] + 9;	/* BR -> BR     */

	
#ifdef SPANISH_SP
	spdef->afgain = pDph_t->curspdef[SP_GF]+3;	 /* GH -> GH             */
#else

	spdef->afgain = pDph_t->curspdef[SP_GF];/* GH -> GH         */
#ifdef SW_VOLUME

/*EAB- 10/7/99 lockheed Martin functionality moved to new code base
	Since the system is tuned "hot" the volume is at a max and these controls allow
	it to be reduced. It works by reducing the the gain of the final cascade resonator (different depending
	on what version of the vocal tract is used), and the two other noise source gains GH and GF. You simply need do 
	decrease these all by the same db amount and the results tract with good fidelity */


/* tek 08aug99 
 * adjust the three final gains by the dB amount specified by the
 * user volume command
 */
   (*spdef).r1ca += KS.CurrentVolumeDB;         
   (*spdef).afgain += KS.CurrentVolumeDB;	
   (*spdef).apgain += KS.CurrentVolumeDB;
#endif /*SW_VOLUME*/

#endif
	spdef->rnpgain = pDph_t->curspdef[SP_GN];	/* GN -> GN             */
#ifdef LOWCOMPUTE
	spdef->rnpgain = pDph_t->curspdef[SP_GN]+1;	/* GN -> GN             */
	spdef->azgain = pDph_t->curspdef[SP_GV]+4;	/* GV -> GV             */
#else
	spdef->azgain = pDph_t->curspdef[SP_GV];	/* GV -> GV             */
	spdef->rnpgain = pDph_t->curspdef[SP_GN];	/* GN -> GN             */
#endif
#ifdef SPANISH_SP
	spdef->apgain = pDph_t->curspdef[SP_GH]+1;	/* GH -> GH             */
#else
#ifdef NEW_VOLUME
	spdef->apgain = pDph_t->curspdef[SP_GH]-temp;	/* GH -> GH         */
#else
	spdef->apgain = pDph_t->curspdef[SP_GH];	/* GH -> GH         */
#endif
#endif
#ifndef MSDOS
	spdef->osgain = pDph_t->curspdef[SP_OS];	/* BH Added speaker to packet *//* GH -> GH  */
	spdef->speaker = pDph_t->curspdef[SP_NM];	/* BH Added speaker to packet *//* GH -> GH  */
#endif

#ifdef SEPARATE_PROCESSESdi
	fwrite (&sd, sizeof (short), 1, stdout);
	fwrite (spdef, sizeof (short), SPDEF_PARS, stdout);

	fflush (stdout);
	free (spdef);
#else
/* for(i=0;i<=31;i++) { printf("spdef[%d]=%d \n",i,curspdef[i]); } */
#ifdef DEBUGVSET
	printf ("r4cb is %d \n", spdef->r4cb);
	printf ("r4cc is %d \n", spdef->r4cc);
	printf ("r5cb is %d \n", spdef->r5cb);
	printf ("r5cc is %d \n", spdef->r5cc);
	printf ("r4pb  is %d \n", spdef->r4pb);
	printf ("r5pb  is %d \n", spdef->r5pb);
	printf ("t0jit is %d \n", spdef->t0jit);
	printf ("r5ca  is %d \n", spdef->r5ca);
	printf ("r4ca  is %d \n", spdef->r4ca);
	printf ("r3ca  is %d \n", spdef->r3ca);
	printf ("r2ca  is %d \n", spdef->r2ca);
	printf ("r1ca  is %d \n", spdef->r1ca);
	printf ("nopen1 is %d \n", spdef->nopen1);
	printf ("nopen2 is %d \n", spdef->nopen2);
	printf ("aturb is %d \n", spdef->aturb);
	printf ("fnscale is %d \n", spdef->fnscale);
	printf ("afgain is %d \n", spdef->afgain);
	printf ("rnpgain is %d \n", spdef->rnpgain);
	printf ("azgain is %d \n", spdef->azgain);
	printf ("apgain is %d \n", spdef->apgain);
	printf ("notused is %d \n", 0);	   /* hardwire */
#endif
#ifdef MSDOS
	spcwrite ((unsigned short _far *) spdef);	/* MVP : typecast was unsigned int _far * */
#else
	spcwrite (pKsd_t, (unsigned short _far *) spdef);	/* MVP : typecast was unsigned int _far * */
#endif
#endif
}
/***************************************end of ph_vset.c******************************/
