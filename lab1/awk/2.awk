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
    if (array[2] == "10.10.2006")
    {
      res[i] = array[1]
      i++
    }    
  }  
}
END {
  for (i in res)
    print res[i]
}
