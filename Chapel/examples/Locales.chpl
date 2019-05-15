config const numLocales: int;
const LocaleSpace = {0..numLocales-1};
const Locales: [LocaleSpace] locale;

./chpl.out --numLocales=4



for loc in Locales do
  on loc do
    writeln("Hello from locale #", here.id);

var x = 42;
on x do
  writeln("Executing where x is stored, which is locale #", here.id);
