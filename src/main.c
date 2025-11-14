//#include <tice.h>
#include <stdlib.h>
#include "node.h"


enum {
	MEM_SIZE = 
#	ifdef TICE_H
		1U << 13
#	else
		1U << 20
#	endif
};
uint8_t backing_mem[MEM_SIZE];

int main(void) {
#ifdef TICE_H
#	warning "compiling for TI84 Calc"
	os_ClrLCDFull();
#else
#	warning "compiling for PC/Other"
#endif
	puts("Welcome to LiteSpiCE");
	struct Circuit circuit = circuit_make(sizeof backing_mem, backing_mem);
#ifdef TICE_H
	puts("Press 'enter' to continue.");
	while( os_GetCSC() != sk_Enter );
	os_ClrLCDFull();
	char num_buff[64] = {0};
	printf("%s\n", rat_to_cstr(os_Int24ToReal(10), sizeof num_buff, num_buff));
	circuit_solve_dc(&circuit);
	puts("Calc'ed Voltages. Press 'enter' to continue.");
	while( os_GetCSC() != sk_Enter );
	os_ClrLCDFull();
	puts("Press 'clear' to Exit.");
	while( os_GetCSC() != sk_Clear );
#else
	/// fill the circuit.
	enum{ COMP_ENTRY_CSTR_LEN = 100 };
	for(;;) {
		char comp_entry[COMP_ENTRY_CSTR_LEN] = {0};
		puts("Enter components for calculation (ex. R n1 n2 ohms, empty to stop, 'q' to exit):: ");
		fgets(&comp_entry[0], sizeof comp_entry - 1, stdin);
		size_t const l = strlen(comp_entry);
		comp_entry[l - 1] = 0;
		if( tolower(comp_entry[0])=='q' ) {
			puts("Exiting LiteSpiCE...");
			break;
		} else if( comp_entry[0]==0 ) {
			break;
		}
		printf("entry:: '%s'\n", comp_entry);
		circuit_add_from_line(&circuit, comp_entry);
	}
	circuit_solve_dc(&circuit);
#endif
}


/** CircuitTutor
 * Improve UI on waveforms.
 * Fix rounding issues.
 * Improve Laplace transform problems.
 * Allow professors to set rounding accuracy percentage. 
 * put "sin" function.
 */

/**
* do R_th
* for t+, turn cap into voltage src, make inductor seek voltage as terminals.
* put current variable on cap->volt src
* 
* 

Vocational Rehabilitation

* Need Pell grant info, do NOT accept loans.
* Requires at least 3.0 GPA.
* Keep up monthly appointments follow criteria for them.
* Provide courses and course GPA every end of semester to Jessica Mackie jmackie@.
* Only pays for one class ONCE; If I drop class, I pay for it.
* Up to 120 Credits for BS, 60 Credits for MS.
* Sign up at Student Disability Resource Center at ASU.
* Class Withdrawal Exception related to disability.
	* Drop within deadline to avoid issue.
* Interact with Job Coach.

* End of month start services.
* Work with support coordinator.
* Helps up to doctorate.
* Helps with internship.
* Do not help with summer courses.
* Helps with gas for car/transportation.
	* 40 Miles one way.
	* 80 miles, reimburse parking pass.
	* 67 cents per mile & Provide with receipts to fill.
*/