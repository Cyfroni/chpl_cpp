proc write_5_times(i){
  for x in 1..5 do
    writeln(i);
}


begin write_5_times(1);

begin {
  write_5_times(2);
  write_5_times(3);
}


cobegin {
  write_5_times(1);
  {
    write_5_times(2);
    write_5_times(3);
  }
}


coforall i in 1..3 do
  write_5_times(i);
