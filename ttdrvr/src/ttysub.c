int con_redirect(int x) {
  if (x==0x0501)
    return 0x400;
  else
    return x;
}

tty$setup_ucb(){
  printk("tty$setup_ucb not impl\n");
}

tty$ds_tran(){
printk("tty$ds_tran not impl\n");
}

tty$readerror(){
printk("tty$readerror not impl\n");
}

tty$class_disconnect(){
printk("tty$class_disconnect not impl\n");
}

tty$class_fork(){
printk("tty$class_fork not impl\n");
}

tty$powerfail(){
printk("tty$powerfail not impl\n");
}

