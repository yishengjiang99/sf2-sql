#include <stdint.h>
char *generator[60] = {"Gen_StartAddrOfs", "Gen_EndAddrOfs", "Gen_StartLoopAddrOfs", "Gen_EndLoopAddrOfs", "Gen_StartAddrCoarseOfs", "Gen_ModLFO2Pitch", "Gen_VibLFO2Pitch", "Gen_ModEnv2Pitch", "Gen_FilterFc", "Gen_FilterQ", "Gen_ModLFO2FilterFc", "Gen_ModEnv2FilterFc", "Gen_EndAddrCoarseOfs", "Gen_ModLFO2Vol", "Gen_Unused1", "Gen_ChorusSend", "Gen_ReverbSend", "Gen_Pan", "Gen_Unused2", "Gen_Unused3", "Gen_Unused4", "Gen_ModLFODelay", "Gen_ModLFOFreq", "Gen_VibLFODelay", "Gen_VibLFOFreq", "Gen_ModEnvDelay", "Gen_ModEnvAttack", "Gen_ModEnvHold", "Gen_ModEnvDecay", "Gen_ModEnvSustain", "Gen_ModEnvRelease", "Gen_Key2ModEnvHold", "Gen_Key2ModEnvDecay", "Gen_VolEnvDelay", "Gen_VolEnvAttack", "Gen_VolEnvHold", "Gen_VolEnvDecay", "Gen_VolEnvSustain", "Gen_VolEnvRelease", "Gen_Key2VolEnvHold", "Gen_Key2VolEnvDecay", "Gen_Instrument", "Gen_Reserved1", "Gen_KeyRange", "Gen_VelRange", "Gen_StartLoopAddrCoarseOfs", "Gen_Keynum", "Gen_Velocity", "Gen_Attenuation", "Gen_Reserved2", "Gen_EndLoopAddrCoarseOfs", "Gen_CoarseTune", "Gen_FineTune", "Gen_SampleId", "Gen_SampleModes", "Gen_Reserved3", "Gen_ScaleTune", "Gen_ExclusiveClass", "Gen_OverrideRootKey", "Gen_Dummy"};

typedef uint8_t BYTE;
typedef uint32_t DWORD; // uint32_t;
typedef DWORD FOURCC;
typedef struct
{
	uint8_t lo, hi;
} rangesType; //  Four-character code
typedef struct
{
	FOURCC ckID;	//  A chunk ID identifies the type of data within the chunk.
	DWORD ckSize; // The size of the chunk data in bytes, excluding any pad byte.
	BYTE *ckDATA; // The actual data plus a pad byte if req'd to word align.
} RIFFCHUNKS;
typedef union
{
	rangesType ranges;
	short shAmount;
	unsigned short uAmount;
} genAmountType;

typedef struct
{
	char name[4];
	unsigned int size;
	char sfbk[4];
	char list[4];
} header_t;

typedef struct
{
	unsigned int size;
	char name[4];
} header2_t;
typedef struct
{
	char name[4];
	unsigned int size;
} section_header;
typedef enum
{
	monoSample = 1,
	rightSample = 2,
	leftSample = 4,
	linkedSample = 8,
	RomMonoSample = 0x8001,
	RomRightSample = 0x8002,
	RomLeftSample = 0x8004,
	RomLinkedSample = 0x8008
} SFSampleLink;
typedef struct
{
	char name[4];
	unsigned int size;
	char *data;
} pdta;
typedef struct
{
	char name[20];
	uint16_t pid, bankId, pbagNdx;
	char idc[12];
} phdr;
typedef struct
{
	unsigned short pgen_id, pmod_id;
} pbag;
typedef struct
{
	unsigned short igen_id, imod_id;
} ibag;
typedef struct
{
	unsigned short operator;
	genAmountType val;
} pgen_t;
typedef struct
{
	char data[10];
} pmod;
typedef struct
{
	char name[20];
	unsigned short ibagNdx;
} inst;
typedef struct
{
	char data[10];
} imod;
typedef union
{
	uint8_t hi, lo;
	unsigned short val;
	short word;
} gen_val;

typedef struct
		pgen_t igen;

typedef struct
{
	char name[20];
	uint32_t start, end, startloop, endloop, sampleRate;
	uint8_t originalPitch, pitchCorrection, v2, v3, v4, v5;

} shdrcast;

typedef struct
{
	short delay, attack, hold, decay, release;
	unsigned short sustain;
} envelope;
typedef struct
{
	uint8_t lokey, hikey, lovel, hivel;
	shdrcast *sampl;
	short attentuation, lpf_cutff, lpf_q;
	float pitchAdjust;
	envelope vol, lfomod;
} zone;
typedef struct
{
	phdr *header;
	zone *zones;
} preset;
