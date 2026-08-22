#include "Interoperability.h"
#include <iostream>

using namespace std;

Interoperability::Interoperability(void)
{
}


Interoperability::~Interoperability(void)
{
}


void Interoperability::registerDisplacementMap(GLuint index, DisplacementStage * displacementStage)
{
	//long t1 = displacementStage->getCpuClocks();

	cl_int error = 0;
	displacementStage->displacementMap = cl::Image2DGL(*displacementStage->context,
														CL_MEM_READ_WRITE,
														GL_TEXTURE_2D,
														0,
														index,
														&error);
	if (error > 0)
		cout << "Error: " << error << endl;

	//long t2 = displacementStage->getCpuClocks();
	//std::cout << "Interop::registerDMap: " << (t2 - t1) * 1.0e-9f << endl;
}


void Interoperability::registerSourceMorphing(GLuint index, DisplacementStage * displacementStage)
{
	//static double testes[1000];
	//static int a = 0;

	//long t1 = this->getCpuClocks();

	cl_int error = 0;
	displacementStage->sourceMorphing = cl::Image2DGL(*displacementStage->context,
										  CL_MEM_READ_WRITE,
										  GL_TEXTURE_2D,
										  0,
										  index,
										  &error);
	if (error > 0)
		cout << "Error: " << error << endl;

	//long t2 = this->getCpuClocks();

	//testes[a++] = fabs((t2 - t1) * 1.0e-9f);

	//if (a == 1000)
	//{
	//	for (int j = 0; j < 999; j++)
	//		for (int b = j+1; b < 1000; b++)
	//			if (testes[j] > testes[b])
	//				swap(testes[j], testes[b]);

	//	for (int j = 0; j < 1000; j++)
	//		std::cout << "test[" << j << "]: " << testes[j] << endl;

	//	float sum = 0;
	//	for ( int j = 1; j < 999; j++)
	//		sum += testes[j];

	//	float media = sum / 998.f;

	//	cout << "soma: " << sum << endl;
	//	cout << "media: " << media << endl;

	//	a++;
	//}
}


void Interoperability::registerDestinyMorphing(GLuint index, DisplacementStage * displacementStage)
{
	//long t1 = displacementStage->getCpuClocks();

	cl_int error = 0;
	displacementStage->destinyMorphing = cl::Image2DGL(*displacementStage->context,
														CL_MEM_READ_WRITE,
														GL_TEXTURE_2D,
														0,
														index,
														&error);
	if (error > 0)
		cout << "Error: " << error << endl;

	//long t2 = displacementStage->getCpuClocks();
	//std::cout << "Interop::registerSMorph: " << (t2 - t1) * 1.0e-9f << endl;
}


// inline __int64 Interoperability::getCpuClocks() 
//{  
//	struct { long low, high; } counter; 
// 
//    __asm push EAX 
//    __asm push EDX 
//    __asm __emit 0fh 
//	__asm __emit 031h
//    __asm mov counter.low, EAX 
//    __asm mov counter.high, EDX 
//    __asm pop EDX 
//    __asm pop EAX 
// 
//    return *(__int64 *)(&counter); 
//}
