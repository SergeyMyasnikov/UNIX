BEGIN {
  fn = ""
}
{
  if (fn != FILENAME)
  {
    fn = FILENAME
    split(fn, array, "/")    
  }
  res1[array[2]] += $2
  res2[array[2]]++   
}
END {
  for (i in res1)
    print "Среднее время по бегу " i " - " res1[i]/res2[i] " миллисекунд"
}
