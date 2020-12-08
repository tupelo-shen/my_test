static int cputest()
{
static volatile float x=1.2342374;
static volatile float y=9.153784;
static volatile float z;
printf("cp0 register read test\n");

asm(\
"mfc0 $0,$0;\n" \
"mfc0 $0,$1;\n" \
"mfc0 $0,$2;\n" \
"mfc0 $0,$3;\n" \
"mfc0 $0,$4;\n" \
"mfc0 $0,$5;\n" \
"mfc0 $0,$6;\n" \
"mfc0 $0,$7;\n" \
"mfc0 $0,$8;\n" \
"mfc0 $0,$9;\n" \
"mfc0 $0,$10;\n" \
"mfc0 $0,$11;\n" \
"mfc0 $0,$12;\n" \
"mfc0 $0,$13;\n" \
"mfc0 $0,$14;\n" \
"mfc0 $0,$15;\n" \
"mfc0 $0,$16;\n" \
"mfc0 $0,$17;\n" \
"mfc0 $0,$18;\n" \
"mfc0 $0,$19;\n" \
"mfc0 $0,$20;\n" \
"mfc0 $0,$21;\n" \
"mfc0 $0,$22;\n" \
"mfc0 $0,$23;\n" \
"mfc0 $0,$24;\n" \
"mfc0 $0,$25;\n" \
"mfc0 $0,$26;\n" \
"mfc0 $0,$27;\n" \
"mfc0 $0,$28;\n" \
"mfc0 $0,$29;\n" \
"mfc0 $0,$30;\n" \
"mfc0 $0,$31;\n" \
:::"$2","$3"
	);

printf("cp0 register read test ok\n");
printf("cpu float calculation test\n");
z=x*y*10000;
if((int)z==112979) printf("cpu float calculation test ok\n");
else printf("cpu float calculation test error\n");
return 0;
}
