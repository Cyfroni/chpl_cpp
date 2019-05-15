config const numLocales: int;
const LocaleSpace = {0..numLocales-1};
const Locales: [LocaleSpace] locale;

./chpl.out --numLocales=4
