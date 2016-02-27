BEGIN {
  fn = ""
  i = 1
}
{
  if (fn != FILENAME)
  {
    fn = FILENAME
    split(fn, array, "/")
    split(array[2], array, "_")
    if (array[1] == "400метров")
    {
      res[i] = array[2]
      i++
    }    
  }  
}
END {
  for (i in res)
    print res[i]
}
