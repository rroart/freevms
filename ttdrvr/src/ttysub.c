int con_redirect(int x) {
  if (x==0x0501)
    return 0x400;
  else
    return x;
}
