#ifndef GLSLSCENE_LOOKUP8_H
#define GLSLSCENE_LOOKUP8_H

/*
 * Should only be included inside a cpp and not inside a header file!
 */

/*
lookUpSet will be filled like
0: 00000001   ...00000001
1: 00000002   ...00000010
2: 00000004   ...00000100
3: 00000008   ...00001000
4: 00000010   ...00010000
5: 00000020   ...00100000
6: 00000040   ...01000000
7: 00000080   ...10000000

Used to set a bit like
mIsUsedFlags |= lookUpSet[textureUnit];
 */
static const unsigned int lookUpSet[8] =
{
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80
};

/*
lookUpReset will be filled like
0: fffffffe   ...11111110
1: fffffffd   ...11111101
2: fffffffb   ...11111011
3: fffffff7   ...11110111
4: ffffffef   ...11101111
5: ffffffdf   ...11011111
6: ffffffbf   ...10111111
7: ffffff7f   ...01111111

Used to reset a bit like
mIsUsedFlags &= lookUpReset[textureUnit];
 */
static const unsigned int lookUpReset[8] =
{
	~lookUpSet[0],
	~lookUpSet[1],
	~lookUpSet[2],
	~lookUpSet[3],
	~lookUpSet[4],
	~lookUpSet[5],
	~lookUpSet[6],
	~lookUpSet[7]
};

/*
lookUpLimit will be filled like
0: 00000001   ...00000001
1: 00000003   ...00000011
2: 00000007   ...00000111
3: 0000000f   ...00001111
4: 0000001f   ...00011111
5: 0000003f   ...00111111
6: 0000007f   ...01111111
7: 000000ff   ...11111111
 */
static const unsigned int lookUpLimit[8] =
{
	0x01,
	0x03,
	0x07,
	0x0f,
	0x1f,
	0x3f,
	0x7f,
	0xff
};

/*
lookUpInvertLimit will be filled like
0: fffffffe   ...11111110
1: fffffffc   ...11111100
2: fffffff8   ...11111000
3: fffffff0   ...11110000
4: ffffffe0   ...11100000
5: ffffffc0   ...11000000
6: ffffff80   ...10000000
7: ffffff00   ...00000000
 */
static const unsigned int lookUpInvertLimit[8] =
{
	~lookUpLimit[0],
	~lookUpLimit[1],
	~lookUpLimit[2],
	~lookUpLimit[3],
	~lookUpLimit[4],
	~lookUpLimit[5],
	~lookUpLimit[6],
	~lookUpLimit[7]
};

#endif /* GLSLSCENE_LOOKUP8_H */
