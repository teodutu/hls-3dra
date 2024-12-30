#include "comp.h"
#include <iostream>
using namespace std;

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

ap_int<32> ALU(ap_int<32> i1, ap_int<32> i2, ap_int<5> opcode)
{
	ap_int<32> res;

	switch (opcode)
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
	}
	cout << "output: " << res << endl;
	return res;
}
