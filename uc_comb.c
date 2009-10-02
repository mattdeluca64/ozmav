/*	------------------------------------------------------------
	OZMAV - OpenGL Zelda Map Viewer

	Written 2008/2009 by xdaniel & contributors
	http://ozmav.googlecode.com/
	------------------------------------------------------------
	uc_comb.c - F3DEX2 coloring & combiner functions
	------------------------------------------------------------ */

#include "globals.h"
#include "debug.h"

/*	------------------------------------------------------------ */

int F3DEX2_Cmd_SETCOMBINE()
{
	Combine0 =	(Readout_CurrentByte2 * 0x10000) +
				(Readout_CurrentByte3 * 0x100) +
				(Readout_CurrentByte4);

	Combine1 =	(Readout_CurrentByte5 * 0x1000000) +
				(Readout_CurrentByte6 * 0x10000) +
				(Readout_CurrentByte7 * 0x100) +
				Readout_CurrentByte8;

	F3DEX2_BuildFragmentShader();

	return 0;
}

int F3DEX2_BuildFragmentShader()
{
	if(!(GLExtension_FragProgram) && !(Renderer_EnableFragShader)) {
		return 0;
	}

	/* fragment program cache management */
	int CacheCheck = 0; bool SearchingCache = true; bool NewProg = false;

	fprintf(FileCombinerLog, "----------------------------------------------------------------------------------------\n"
		"SETCOMBINE -- Raw data: 0x%08X 0x%08X\n", Combine0, Combine1);

	while(SearchingCache) {
		if((FPCache[CacheCheck].Combine0 == Combine0) && (FPCache[CacheCheck].Combine1 == Combine1)) {
			/* if hit found, exit check loop */
			SearchingCache = false;
			NewProg = false;
			fprintf(FileCombinerLog, " -> corresponding fragment program found, cache slot %d\n", CacheCheck);
			fprintf(FileCombinerLog, "----------------------------------------------------------------------------------------\n\n");
		} else {
			/* if no hit... */
			if(CacheCheck != 256) {
				/* if not at limit, increase pos */
				CacheCheck++;
			} else {
				/* if at limit, exit check loop and gen new prog */
				SearchingCache = false;
				NewProg = true;
				fprintf(FileCombinerLog, " -> creating new fragment program, cache slot %d\n", FPCachePosition);
			}
		}
	}

	if(NewProg) {
		/* gen new prog */
		cA[0] = ((Combine0 >> 20) & 0x0F);
		cB[0] = ((Combine1 >> 28) & 0x0F);
		cC[0] = ((Combine0 >> 15) & 0x1F);
		cD[0] = ((Combine1 >> 15) & 0x07);

		aA[0] = ((Combine0 >> 12) & 0x07);
		aB[0] = ((Combine1 >> 12) & 0x07);
		aC[0] = ((Combine0 >>  9) & 0x07);
		aD[0] = ((Combine1 >>  9) & 0x07);

		cA[1] = ((Combine0 >>  5) & 0x0F);
		cB[1] = ((Combine1 >> 24) & 0x0F);
		cC[1] = ((Combine0 >>  0) & 0x1F);
		cD[1] = ((Combine1 >>  6) & 0x07);

		aA[1] = ((Combine1 >> 21) & 0x07);
		aB[1] = ((Combine1 >>  3) & 0x07);
		aC[1] = ((Combine1 >> 18) & 0x07);
		aD[1] = ((Combine1 >>  0) & 0x07);

		char CombMsg[1024];
		sprintf(CombMsg, "----------------------------------------------------------------------------------------\n"
			"Color0: [(%s - %s) * %s] + %s\n"
			"Alpha0: [(%s - %s) * %s] + %s\n"
			"Color1: [(%s - %s) * %s] + %s\n"
			"Alpha1: [(%s - %s) * %s] + %s\n"
			"----------------------------------------------------------------------------------------\n",
			CombinerTypes16[cA[0]], CombinerTypes16[cB[0]], CombinerTypes32[cC[0]], CombinerTypes8[cD[0]],
			CombinerTypes8[aA[0]], CombinerTypes8[aB[0]], CombinerTypes8[aC[0]], CombinerTypes8[aD[0]],
			CombinerTypes16[cA[1]], CombinerTypes16[cB[1]], CombinerTypes32[cC[1]], CombinerTypes8[cD[1]],
			CombinerTypes8[aA[1]], CombinerTypes8[aB[1]], CombinerTypes8[aC[1]], CombinerTypes8[aD[1]]);
		fprintf(FileCombinerLog, CombMsg);

		char * LeadIn =
			"!!ARBfp1.0\n"
			"\n"
			"%s"
			"TEMP Tex0; TEMP Tex1;\n"
			"TEMP R0; TEMP R1;\n"
			"TEMP aR0; TEMP aR1;\n"
			"TEMP Comb; TEMP aComb;\n"
			"\n"
			"PARAM EnvColor = program.env[0];\n"
			"PARAM PrimColor = program.env[1];\n"
			"PARAM BlendColor = program.env[2];\n"
			"ATTRIB Shade = fragment.color.primary;\n"
			"\n"
			"OUTPUT Out = result.color;\n"
			"\n"
			"TEX Tex0, fragment.texcoord[0], texture[0], 2D;\n"
			"TEX Tex1, fragment.texcoord[1], texture[1], 2D;\n"
			"\n";

		memset(ShaderString, 0x00, sizeof(ShaderString));
		sprintf(ShaderString, LeadIn, (Renderer_EnableFog ? "OPTION ARB_fog_linear;\n" : "\n"));

		int Cycle = 0, NumCycles = 1;
		if(RDPCycleMode == G_CYC_2CYCLE) NumCycles = 2;

		NumCycles = 2;		/* never set to G_CYC_2CYCLE by map dlists, so force manually because many combiner modes require 2 cycles! */

		for(Cycle = 0; Cycle < NumCycles; Cycle++) {
			sprintf(ShaderString, "%s# Color%d\n", ShaderString, Cycle);
			switch(cA[Cycle]) {
				case G_CCMUX_COMBINED:
					strcat(ShaderString, "MOV R0.rgb, Comb;\n");
					break;
				case G_CCMUX_TEXEL0:
					strcat(ShaderString, "MOV R0.rgb, Tex0;\n");
					break;
				case G_CCMUX_TEXEL1:
					strcat(ShaderString, "MOV R0.rgb, Tex1;\n");
					break;
				case G_CCMUX_PRIMITIVE:
					strcat(ShaderString, "MOV R0.rgb, PrimColor;\n");
					break;
				case G_CCMUX_SHADE:
					strcat(ShaderString, "MOV R0.rgb, Shade;\n");
					break;
				case G_CCMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV R0.rgb, EnvColor;\n");
					break;
				case G_CCMUX_1:
					strcat(ShaderString, "MOV R0.rgb, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case G_CCMUX_COMBINED_ALPHA:
					strcat(ShaderString, "MOV R0.rgb, Comb.a;\n");
					break;
				case G_CCMUX_TEXEL0_ALPHA:
					strcat(ShaderString, "MOV R0.rgb, Tex0.a;\n");
					break;
				case G_CCMUX_TEXEL1_ALPHA:
					strcat(ShaderString, "MOV R0.rgb, Tex1.a;\n");
					break;
				case G_CCMUX_PRIMITIVE_ALPHA:
					strcat(ShaderString, "MOV R0.rgb, PrimColor.a;\n");
					break;
				case G_CCMUX_SHADE_ALPHA:
					strcat(ShaderString, "MOV R0.rgb, Shade.a;\n");
					break;
				case G_CCMUX_ENV_ALPHA:
					strcat(ShaderString, "MOV R0.rgb, EnvColor.a;\n");
					break;
				case G_CCMUX_LOD_FRACTION:
					strcat(ShaderString, "MOV R0.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
					break;
				case G_CCMUX_PRIM_LOD_FRAC:
					strcat(ShaderString, "MOV R0.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
					break;
				case 15:	// 0
					strcat(ShaderString, "MOV R0.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV R0.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s# -%d\n", ShaderString, cA[Cycle]);
					break;
			}

			switch(cB[Cycle]) {
				case G_CCMUX_COMBINED:
					strcat(ShaderString, "MOV R1.rgb, Comb;\n");
					break;
				case G_CCMUX_TEXEL0:
					strcat(ShaderString, "MOV R1.rgb, Tex0;\n");
					break;
				case G_CCMUX_TEXEL1:
					strcat(ShaderString, "MOV R1.rgb, Tex1;\n");
					break;
				case G_CCMUX_PRIMITIVE:
					strcat(ShaderString, "MOV R1.rgb, PrimColor;\n");
					break;
				case G_CCMUX_SHADE:
					strcat(ShaderString, "MOV R1.rgb, Shade;\n");
					break;
				case G_CCMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV R1.rgb, EnvColor;\n");
					break;
				case G_CCMUX_1:
					strcat(ShaderString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case G_CCMUX_COMBINED_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Comb.a;\n");
					break;
				case G_CCMUX_TEXEL0_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Tex0.a;\n");
					break;
				case G_CCMUX_TEXEL1_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Tex1.a;\n");
					break;
				case G_CCMUX_PRIMITIVE_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, PrimColor.a;\n");
					break;
				case G_CCMUX_SHADE_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Shade.a;\n");
					break;
				case G_CCMUX_ENV_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, EnvColor.a;\n");
					break;
				case G_CCMUX_LOD_FRACTION:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
					break;
				case G_CCMUX_PRIM_LOD_FRAC:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
					break;
				case 15:	// 0
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s# -%d\n", ShaderString, cB[Cycle]);
					break;
			}
			strcat(ShaderString, "SUB R0, R0, R1;\n\n");

			switch(cC[Cycle]) {
				case G_CCMUX_COMBINED:
					strcat(ShaderString, "MOV R1.rgb, Comb;\n");
					break;
				case G_CCMUX_TEXEL0:
					strcat(ShaderString, "MOV R1.rgb, Tex0;\n");
					break;
				case G_CCMUX_TEXEL1:
					strcat(ShaderString, "MOV R1.rgb, Tex1;\n");
					break;
				case G_CCMUX_PRIMITIVE:
					strcat(ShaderString, "MOV R1.rgb, PrimColor;\n");
					break;
				case G_CCMUX_SHADE:
					strcat(ShaderString, "MOV R1.rgb, Shade;\n");
					break;
				case G_CCMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV R1.rgb, EnvColor;\n");
					break;
				case G_CCMUX_1:
					strcat(ShaderString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case G_CCMUX_COMBINED_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Comb.a;\n");
					break;
				case G_CCMUX_TEXEL0_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Tex0.a;\n");
					break;
				case G_CCMUX_TEXEL1_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Tex1.a;\n");
					break;
				case G_CCMUX_PRIMITIVE_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, PrimColor.a;\n");
					break;
				case G_CCMUX_SHADE_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, Shade.a;\n");
					break;
				case G_CCMUX_ENV_ALPHA:
					strcat(ShaderString, "MOV R1.rgb, EnvColor.a;\n");
					break;
				case G_CCMUX_LOD_FRACTION:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
					break;
				case G_CCMUX_PRIM_LOD_FRAC:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");	// unemulated
					break;
				case G_CCMUX_K5:
					strcat(ShaderString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");	// unemulated
					break;
				case G_CCMUX_0:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s- #%d\n", ShaderString, cC[Cycle]);
					break;
			}
			strcat(ShaderString, "MUL R0, R0, R1;\n\n");

			switch(cD[Cycle]) {
				case G_CCMUX_COMBINED:
					strcat(ShaderString, "MOV R1.rgb, Comb;\n");
					break;
				case G_CCMUX_TEXEL0:
					strcat(ShaderString, "MOV R1.rgb, Tex0;\n");
					break;
				case G_CCMUX_TEXEL1:
					strcat(ShaderString, "MOV R1.rgb, Tex1;\n");
					break;
				case G_CCMUX_PRIMITIVE:
					strcat(ShaderString, "MOV R1.rgb, PrimColor;\n");
					break;
				case G_CCMUX_SHADE:
					strcat(ShaderString, "MOV R1.rgb, Shade;\n");
					break;
				case G_CCMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV R1.rgb, EnvColor;\n");
					break;
				case G_CCMUX_1:
					strcat(ShaderString, "MOV R1.rgb, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case 7:		// 0
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV R1.rgb, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s# -%d\n", ShaderString, cD[Cycle]);
					break;
			}
			strcat(ShaderString, "ADD R0, R0, R1;\n\n");

			sprintf(ShaderString, "%s# Alpha%d\n", ShaderString, Cycle);

			switch(aA[Cycle]) {
				case G_ACMUX_COMBINED:
					strcat(ShaderString, "MOV aR0.a, aComb;\n");
					break;
				case G_ACMUX_TEXEL0:
					strcat(ShaderString, "MOV aR0.a, Tex0;\n");
					break;
				case G_ACMUX_TEXEL1:
					strcat(ShaderString, "MOV aR0.a, Tex1;\n");
					if((Combine0 == 0x00121603) && (Combine1 == 0xFF5BFFF8)) strcat(ShaderString, "MOV aR0.a, {0.0, 0.0, 0.0, 0.0}; # pathway hack (T1->0)\n");
					break;
				case G_ACMUX_PRIMITIVE:
					strcat(ShaderString, "MOV aR0.a, PrimColor;\n");
					break;
				case G_ACMUX_SHADE:
					strcat(ShaderString, "MOV aR0.a, Shade;\n");
					break;
				case G_ACMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV aR0.a, EnvColor;\n");
					break;
				case G_ACMUX_1:
					strcat(ShaderString, "MOV aR0.a, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case G_ACMUX_0:
					strcat(ShaderString, "MOV aR0.a, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV aR0.a, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s# -%d\n", ShaderString, aA[Cycle]);
					break;
			}

			switch(aB[Cycle]) {
				case G_ACMUX_COMBINED:
					strcat(ShaderString, "MOV aR1.a, aComb.a;\n");
					break;
				case G_ACMUX_TEXEL0:
					strcat(ShaderString, "MOV aR1.a, Tex0.a;\n");
					break;
				case G_ACMUX_TEXEL1:
					strcat(ShaderString, "MOV aR1.a, Tex1.a;\n");
					break;
				case G_ACMUX_PRIMITIVE:
					strcat(ShaderString, "MOV aR1.a, PrimColor.a;\n");
					break;
				case G_ACMUX_SHADE:
					strcat(ShaderString, "MOV aR1.a, Shade.a;\n");
					break;
				case G_ACMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV aR1.a, EnvColor.a;\n");
					break;
				case G_ACMUX_1:
					strcat(ShaderString, "MOV aR1.a, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case G_ACMUX_0:
					strcat(ShaderString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s# -%d\n", ShaderString, aB[Cycle]);
					break;
			}
			strcat(ShaderString, "SUB aR0.a, aR0.a, aR1.a;\n\n");

			switch(aC[Cycle]) {
				case G_ACMUX_COMBINED:
					strcat(ShaderString, "MOV aR1.a, aComb.a;\n");
					break;
				case G_ACMUX_TEXEL0:
					strcat(ShaderString, "MOV aR1.a, Tex0.a;\n");
					break;
				case G_ACMUX_TEXEL1:
					strcat(ShaderString, "MOV aR1.a, Tex1.a;\n");
					break;
				case G_ACMUX_PRIMITIVE:
					strcat(ShaderString, "MOV aR1.a, PrimColor.a;\n");
					break;
				case G_ACMUX_SHADE:
					strcat(ShaderString, "MOV aR1.a, Shade.a;\n");
					break;
				case G_ACMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV aR1.a, EnvColor.a;\n");
					break;
				case G_ACMUX_1:
					strcat(ShaderString, "MOV aR1.a, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case G_ACMUX_0:
					strcat(ShaderString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s# -%d\n", ShaderString, aC[Cycle]);
					break;
			}
			strcat(ShaderString, "MUL aR0.a, aR0.a, aR1.a;\n\n");

			switch(aD[Cycle]) {
				case G_ACMUX_COMBINED:
					strcat(ShaderString, "MOV aR1.a, aComb.a;\n");
					break;
				case G_ACMUX_TEXEL0:
					strcat(ShaderString, "MOV aR1.a, Tex0.a;\n");
					break;
				case G_ACMUX_TEXEL1:
					strcat(ShaderString, "MOV aR1.a, Tex1.a;\n");
					break;
				case G_ACMUX_PRIMITIVE:
					strcat(ShaderString, "MOV aR1.a, PrimColor.a;\n");
					break;
				case G_ACMUX_SHADE:
					strcat(ShaderString, "MOV aR1.a, Shade.a;\n");
					break;
				case G_ACMUX_ENVIRONMENT:
					strcat(ShaderString, "MOV aR1.a, EnvColor.a;\n");
					break;
				case G_ACMUX_1:
					strcat(ShaderString, "MOV aR1.a, {1.0, 1.0, 1.0, 1.0};\n");
					break;
				case G_ACMUX_0:
					strcat(ShaderString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
					break;
				default:
					strcat(ShaderString, "MOV aR1.a, {0.0, 0.0, 0.0, 0.0};\n");
					sprintf(ShaderString, "%s# -%d\n", ShaderString, aD[Cycle]);
					break;
			}
			strcat(ShaderString, "ADD aR0.a, aR0.a, aR1.a;\n\n");

			strcat(ShaderString, "MOV Comb.rgb, R0;\n");
			strcat(ShaderString, "MOV aComb.a, aR0.a;\n\n");
		}

		strcat(ShaderString, "# Finish\n");
		strcat(ShaderString,
				"MOV Comb.a, aComb.a;\n"
				"MOV Out, Comb;\n"
				"END\n");

		fprintf(FileCombinerLog, ShaderString);

		fprintf(FileCombinerLog, "----------------------------------------------------------------------------------------\n\n");

		/* basic multitexture - water, ground, certain walls */
		if(((Combine0 == 0x00267E04) && (Combine1 == 0x1F0CFDFF)) || ((Combine0 == 0x00267E04) && (Combine1 == 0x1FFCFDF8))) {
			//sprintf(ShaderString, "");
		}

		/* pathways (ex. spot00 dlist #33) */
		if((Combine0 == 0x00121603) && (Combine1 == 0xFF5BFFF8)) {
			//sprintf(ShaderString, "");
		}

		F3DEX2_ForceBlender();

		if((GLExtension_FragProgram) && (Renderer_EnableFragShader)) {
			glEnable(GL_FRAGMENT_PROGRAM_ARB);
			glGenProgramsARB(1, &FPCache[FPCachePosition].FragProg);
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FPCache[FPCachePosition].FragProg);
			glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(ShaderString), ShaderString);

			if(glGetError() != 0) {
				int Pos = 0;
				char *String = (char *)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
				glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &Pos);
				if(Pos >= 0) {
					sprintf(ErrorMsg, "%s\n%s\n", String, ShaderString + Pos);
					MessageBox(hwnd, ErrorMsg, "Fragment Program Error", MB_OK | MB_ICONEXCLAMATION);
					glDisable(GL_FRAGMENT_PROGRAM_ARB);
					return -1;
				}
			}

			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FPCache[FPCachePosition].FragProg);

			FPCache[FPCachePosition].Combine0 = Combine0;
			FPCache[FPCachePosition].Combine1 = Combine1;
			FPCachePosition++;
		}
	} else {
		/* get existing prog */
		if((GLExtension_FragProgram) && (Renderer_EnableFragShader)) {
			glEnable(GL_FRAGMENT_PROGRAM_ARB);
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, FPCache[CacheCheck].FragProg);
		}
	}

	return 0;
}

int F3DEX2_Cmd_SETFOGCOLOR()
{
	FogColor[0] = (Readout_CurrentByte5 / 255.0f);
	FogColor[1] = (Readout_CurrentByte6 / 255.0f);
	FogColor[2] = (Readout_CurrentByte7 / 255.0f);
	FogColor[3] = (Readout_CurrentByte8 / 255.0f);

	glFogfv(GL_FOG_COLOR, FogColor);
	glClearColor(FogColor[0], FogColor[1], FogColor[2], FogColor[3]);

	return 0;
}

int F3DEX2_Cmd_SETBLENDCOLOR()
{
	BlendColor[0] = (Readout_CurrentByte5 / 255.0f);
	BlendColor[1] = (Readout_CurrentByte6 / 255.0f);
	BlendColor[2] = (Readout_CurrentByte7 / 255.0f);
	BlendColor[3] = (Readout_CurrentByte8 / 255.0f);

	if(GLExtension_FragProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 2, BlendColor[0], BlendColor[1], BlendColor[2], BlendColor[3]);
	}

	return 0;
}

int F3DEX2_Cmd_SETPRIMCOLOR()
{
	PrimColor[0] = (Readout_CurrentByte5 / 255.0f);
	PrimColor[1] = (Readout_CurrentByte6 / 255.0f);
	PrimColor[2] = (Readout_CurrentByte7 / 255.0f);
	PrimColor[3] = (Readout_CurrentByte8 / 255.0f);

	if(GLExtension_FragProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 1, PrimColor[0], PrimColor[1], PrimColor[2], PrimColor[3]);
	}

	return 0;
}

int F3DEX2_Cmd_SETENVCOLOR()
{
	EnvColor[0] = (Readout_CurrentByte5 / 255.0f);
	EnvColor[1] = (Readout_CurrentByte6 / 255.0f);
	EnvColor[2] = (Readout_CurrentByte7 / 255.0f);
	EnvColor[3] = (Readout_CurrentByte8 / 255.0f);

	if(GLExtension_FragProgram) {
		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, EnvColor[0], EnvColor[1], EnvColor[2], EnvColor[3]);
	}

	return 0;
}
