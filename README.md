# uHeap

C++ Dynamic memory management for microcontrollers(or not).

## Features

  - Implemented as a singleton object with static lifetime and memory allocation.
  - OS-independent realization. You can provide your own locking mechanism, debug output etc.
  - Implementation is based on a single-linked list of free memory blocks and provides O(1) complexity in most cases and O(n) in worst.
  - In most cases twice as fast as malloc 
  - Less memory fragmentation than standard malloc. 

## Usage

Define size of heap in project `#define UHEAP_HEAP_SIZE (size)`

Using singleton interfase

``` c++
    Type* pointer = ufw::uHeap::instance().allocate(sizeof(Type)); // to allocate "pointer" of "Type"
    ufw::uHeap::instance().deallocate(pointer); // to deallocate
```

  - uHeap can be built with global new/delete-operators overriding implementation. Just add `#define UHEAP_OVERRIDES_NEW 1` to your project
  - uHeap can global override malloc-functions. You must define `UHEAP_WRAPS_MALLOC` and add `-Xlinker --wrap=malloc` linker options

For more information about options read uheap_opt.h options descriptions.

## License

"uHeap" is licensed under the terms of MIT license. Be free to use, modify, fork, etc.

Copyright (c) 2016-2021 by Dmitry Donskih. All rights reserved.

## How to Contribute
  - Fork it
  - Create your feature branch (git checkout -b my-new-feature)
  - Commit your changes (git commit -am 'Add some feature')
  - Push to the branch (git push origin my-new-feature)
  - Create new Pull Request
