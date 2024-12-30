#include "comp.h"
#include <iostream>
using namespace std;

#define PREDICATION_BIT				6
#define PREDICATION_BIT_MASK		(1 << PREDICATION_BIT)

ap_int<32> ADD(ap_int<16> i1, ap_int<16> i2)
{
	// i1=(ap_int<16>)i1;
	ap_int<32> res = i1 + i2;
#pragma HLS BIND_OP variable = res op = add impl = dsp
	return res;
}

ap_int<32> MUL(ap_int<32> i1, ap_int<32> i2)
{

	ap_int<32> res = i1 * i2;
#pragma HLS BIND_OP variable = res op = mul impl = dsp
	return res;
}

ap_int<32> SUB(ap_int<32> i1, ap_int<32> i2)
{
	return ADD(i1, -i2);
}

ap_int<32> AND32(ap_int<32> i1, ap_int<32> i2)
{
	return i1 & i2;
}

ap_int<32> OR32(ap_int<32> i1, ap_int<32> i2)
{
	return i1 | i2;
}

ap_int<32> XOR32(ap_int<32> i1, ap_int<32> i2)
{
	return i1 ^ i2;
}

bool CGT(ap_int<32> i1, ap_int<32> i2)
{
	return i1 > i2;
}

bool CLT(ap_int<32> i1, ap_int<32> i2)
{
	return i1 < i2;
}

bool CMP(ap_int<32> i1, ap_int<32> i2)
{
	return i1 != i2;
}

ap_int<32> SELECT(ap_int<32> i1, ap_int<32> i2, ap_int<1> pred)
{
	return pred ? i1 : i2;
}

//Add two more operation, load and store as 12 and 13
ap_int<32> ALU(ap_int<32> i1, ap_int<32> i2, ap_int<32> constant, ap_int<OPCODE_WIDTH> opcode,
				ap_int<1>& mem_req, ap_int<1>& mem_read_write, ap_int<32>&address, ap_int<32>& data)
{
	ap_int<32> res = 0;
	mem_req = 0;
	mem_read_write = 0;

	if (opcode == (14 | PREDICATION_BIT_MASK)) {
		res = SELECT(i1, i2, opcode & PREDICATION_BIT_MASK);
		goto output;
	}

	if (opcode & PREDICATION_BIT_MASK)
		return 0;

	switch (opcode & ~PREDICATION_BIT_MASK)
	{
	case 0:
		res = ADD(i1, i2); // ADD(i1, i2);
		break;
	case 1:
		res = MUL(i1, i2); // MUL(i1, i2);
		break;
	case 2:
		res = SUB(i1, i2); // SUB(i1, i2);
		break;
	case 3:
		res = AND32(i1, i2); // AND32(i1, i2);
		break;
	case 4:
		res = OR32(i1, i2); // OR32(i1, i2);
		break;
	case 5:
		res = XOR32(i1, i2); // XOR32(i1, i2);
		break;
	case 6:
		res = CGT(i1, i2); // CGT(i1, i2);
		break;
	case 7:
		res = CLT(i1, i2); // CLT(i1, i2);
		break;
	case 8:
		res = CMP(i1, i2); // CMP(i1, i2);
		break;
	case 11:
		res = i2; // MOV i1
		break;
	case 12:
		address = constant; //Load
		mem_req = 1;
		mem_read_write = 1;
		break;
	case 13:
		address = constant; //store 
		data = i2;
		mem_req = 1;
		mem_read_write = 0;
	}

output:
	cout << "output: " << res << endl;
	return res;
}
