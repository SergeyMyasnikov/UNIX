BEGIN {
  fn = ""
}
{
  if (fn != FILENAME)
  {
    fn = FILENAME
    split(fn, array, "/")
    min[array[2]] = $2
    name[array[2]] = $1    
  }
  if ($2 < min[array[2]])
  {
    min[array[2]] = $2
    name[array[2]] = $1
  }
}
END {
  for (i in min)
    print "Победитель по бегу " i " - " name[i]
}
