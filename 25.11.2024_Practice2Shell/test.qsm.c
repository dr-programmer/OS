#include <stdio.h>
#include <math.h>
#include <time.h>
#define SMART_DEALLOCATION
#include "qbit.h"

int main() {S
srand(time(0));
quantum_gate *I = quantum_gate_create(2);
INDEX(I, 0, 0) = complex_create(1.000000, 0.000000);
INDEX(I, 0, 1) = complex_create(0.000000, 0.000000);
INDEX(I, 1, 0) = complex_create(0.000000, 0.000000);
INDEX(I, 1, 1) = complex_create(1.000000, 0.000000);
quantum_gate *X = quantum_gate_create(2);
INDEX(X, 0, 0) = complex_create(0.000000, 0.000000);
INDEX(X, 0, 1) = complex_create(1.000000, 0.000000);
INDEX(X, 1, 0) = complex_create(1.000000, 0.000000);
INDEX(X, 1, 1) = complex_create(0.000000, 0.000000);
quantum_state *t0 = matrix_create_empty(4, 1);
INDEX(t0, 0, 0) = complex_create(1.000000, 0.000000);
INDEX(t0, 1, 0) = complex_create(0.000000, 0.000000);
INDEX(t0, 2, 0) = complex_create(0.000000, 0.000000);
INDEX(t0, 3, 0) = complex_create(0.000000, 0.000000);
struct matrix *t1 = matrix_create_empty(1, 1);
INDEX(t1, 0, 0) = complex_create(0.000000, 0.000000);
quantum_gate *t2 = NULL;
{
const unsigned int qubits_count = log2(t0->rows);
const unsigned int preceding_qubits_count = INDEX(t1, 0, 0).real;
const unsigned int measured_qubits_count = 
	(INDEX(t1, 0, 0).real - INDEX(t1, 0, 0).real) + 1;
const unsigned int proceeding_qubits_count = 
	qubits_count - (INDEX(t1, 0, 0).real + 1);
if(preceding_qubits_count) t2 = 
	quantum_gate_create(pow(2, preceding_qubits_count));
if(t2 == NULL) t2 = X;
else t2 = matrix_tensor_product(t2, X);
if(proceeding_qubits_count) t2 = 
	matrix_tensor_product(t2, 
		quantum_gate_create(pow(2, proceeding_qubits_count))
	);
}
quantum_state *t3 = matrix_mul(t2, t0);
qm_result *t4 = quantum_state_measure_subsystem(t3, 0, 0);
quantum_state *t5 = t4->state;
printf(CYN"Classical bit representation: "GRN"%d \n"RESET, t4->value);
qm_result *t6 = quantum_state_measure(t5);
quantum_state *t7 = t6->state;
printf(CYN"Classical bit representation: "GRN"%d \n"RESET, t6->value);
E
return 0;
}
