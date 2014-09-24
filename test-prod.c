// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `prod` and `array_prod` functions.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// **TODO**: Check the performance an 1024 and 2048 bit integer array.

// **Test `prod`**.
static char * test_factorial(size_t size, char * expect) {
	mpz_t p;
	mpz_t e;
	mpz_t *arr;
	size_t g;
	
	// Initialize `p` and `e`.
	mpz_init(p);
	mpz_init_set_str(e, expect, 10);
	
	// Allocate the memory for the array.
	arr = malloc(size*sizeof(mpz_t));
	if (arr == NULL) {
		return "error malloc = NULL";
	}
	
	// Set the integers in the array.
	for(g=1;g<=size;g++) {
		mpz_init_set_ui(arr[g-1], g);
	}
	
	// Calculate the product of the array.
	prod(p, arr, 0, size-1);
	
	// Check the return value.
	if (mpz_cmp(p, e) != 0) {
		return "calculation error";
	}
	// Check if the array is unmodified.
	for(g=0; g<size; g++) {
		if (mpz_get_ui(arr[g]) != (g+1))
			return "array is modified";
	}

	// Free memory.
	mpz_clear(p);
	mpz_clear(e);
	
	for(g=0;g<size;g++) {
		mpz_clear(arr[g]);
	}
	free(arr);

	return 0;
}

// **Test `prod` and the `array` wrapper**.
static char * test_array_prod() {
	mpz_array a;
	mpz_t p;
	mpz_t e;
	mpz_t r;
	size_t g;
	
	// Initialize `r` and `p`.
	mpz_init(r);
	mpz_init(p);
	// 10! = 3628800
	mpz_init_set_str(e, "3628800", 10);
	// Create an to small array.
	array_init(&a, 4);
	
	// Add the ingeres 1 to 10.
	for(g=1;g<=10;g++) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}
	
	// Calculate the product of the array.
	array_prod(&a, r);
	
	// Check the return value.
	if (mpz_cmp(r, e) != 0) {
		return mpz_get_str(NULL, 10, r);
	}
	
	// Check if the array is unmodified.
	for(g=0; g<a.used; g++) {
		if (mpz_get_ui(a.array[g]) != (g+1))
			return "array is modified";
	}

	// Free memory.
	array_clear(&a);
	mpz_clear(p);
	mpz_clear(e);
	mpz_clear(r);
	
	return 0;
}

// Run all tests.
int main(int argc, char **argv) {
	
	printf("Starting prod test\n");
	
	printf("Testing 10!                    ");
	test_evaluate(test_factorial(10, "3628800"));
	
	printf("Testing 128!                   ");
	test_evaluate(test_factorial(128, "385620482362580421735677065923463640617493109590223\
	59027882840327637340257516554356068616858850736153403005183305891634759217293226249885\
	7766114955245039357760034644709279247692495585280000000000000000000000000000000"));
	
	printf("Testing 511!                   ");
	test_evaluate(test_factorial(511, "679158162721211985016219905770625920310986456180373\
	93952001461553742390860670948114711129889443403391490672521683866122127930019935560354\
	15315490493377694167162341094623363024629705069482370340382583639770604852051025007751\
	07890088126098656378038715081462473522696873528419842162562552418127764020680024482968\
	79059826849624386667965032367358173809431908950554997014218480162185026961392533555024\
	36868966699600503105213725244434249053312402536660142178787716218331892912089890567615\
	37396764500221195542160060191115032457878894805142233039906616035852229028098305658113\
	90619752032684095172270976285035652939151733750168427720339924200279546132311655195953\
	48486928597217331364955500576411208445051037476983219685497912253874647579721947172586\
	38273203703524026896110956010053969587186130935220793485519208765935248804175472350417\
	31424381559806483665425737450956215200571101573797878962406687241226979994202334868501\
	45255759861617124660957045718318318733087761592391069549905082253655735352487467721745\
	68817163299338489597213440259556693245952000000000000000000000000000000000000000000000\
	000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
	
	printf("Testing 1024!                  ");
	test_evaluate(test_factorial(1024, "54185287960588572830769219446838547380015539635380\
	13444482870270683210612073376603733140984136214586719079188457089807539319941657701873\
	68260454133333721939108367528012764993769768292516937891165755680659663747947314518404\
	88667767255612518869433525121367727452196343077013371320579624843312887008843617165469\
	02375183904529447322778084029321587220618538061628060639254353108221868482392871302616\
	90914211362251144684713888587881629252104046295315949943900357882410243934315037444113\
	89080618140621086395327523537588501859845158222959965455854124278913090248694429861092\
	31533075791316757451464363040248908204429077345618273690305022527969265530729673709907\
	58747793127635104702469889667961462133026237158973227857814631807156427767644064591085\
	07656478345632445773685381033698177608049870776704639427260534141677912569773337456803\
	74751866762659616656158846814502633370425226641418621570468256847733609443267374936766\
	74915098953768112945831626643856479027816385730291542667725665642276826058264393884514\
	91197641967550929020859271315636298329098944105273212518724952750131407167640551693619\
	07818212367019122957673631170541265899299164820085157817519554669109028387292322245099\
	06388638147771255227782631322385756948819393658889908993670874516860653098411020299853\
	81628156433498184710577783953474253149962210348880758451370576983976399310392966504604\
	61211666513451311495136574008690563348678598850256017872849825677873144072165242722629\
	97319791568603629406624740101482697559533155736658800562921274680657285201570401940692\
	28555780061142905575532454979400893984914681263986075008526329882022471958550534477371\
	15906566828210414172650406586006838449451043549988128868013165515517146733883233408517\
	63819713591312372548673734783537316341517369387565212899726597964903241208727348690699\
	80299636926507008875838485454754227277102425504990231927583091815744820519642107283720\
	49372935161753419577754224531524422803913724077178916612030610402558300550338867900521\
	16025408740454620938384367637886658769912790922323717371343176067483352513629123362885\
	89362713229418356588401041872786935443907708527828855830842709046107501900718493313991\
	55582127523923298797806496390753338457191738228405018695704636266002352655875023355954\
	89311637509380219119860471335771652403999403296360245577257963673286654348957325740999\
	71056713162327234576676193765140810399919363390828642051009857745452406810689739249313\
	82873622262579200000000000000000000000000000000000000000000000000000000000000000000000\
	00000000000000000000000000000000000000000000000000000000000000000000000000000000000000\
	00000000000000000000000000000000000000000000000000000000000000000000000000000000000000\
	0000000000"));
	
	printf("Testing array wrapper          ");
	test_evaluate(test_array_prod());

	test_end();
}
