#PPM_Edit
PPM_Edit is a multi-language library which allows users to open PPM, PGM, and PBM files and modify pixel values easily. These formats are rudimentary and store either ASCII or Binary data depending on their magic number.

The Magic Number determines the format. Here is the list:
```
P1 - Portable BitMap (ASCII)
P2 - Portable GrayMap (ASCII)
P3 - Portable PixMap (ASCII)
P4 - Portable BitMap (Binary)
P5 - Portable GrayMap (Binary)
P6 - Portable PixMap (Binary)
P7 - Portable ArbitaryMap (Unknown)
```

In Computer Science, it is a common thing to have assignments with these kinds of files since they are very easy to read and write if you know how the format works. For that, I have <a href = "http://web.eecs.utk.edu/~ssmit285/guide/img/index.html">written a guide on how these formats work</a> on my university page. If you like reading code better than English, you may look at my source code and figure it out that way.

<b>Note</b>: As of now, this does not consider commented (#) lines.

As always, all documentation is on my university page here: <a href = "http://web.eecs.utk.edu/~ssmit285/lib/ppm_edit/cpp/index.html">http://web.eecs.utk.edu/~ssmit285/lib/ppm_edit/cpp/index.html</a>
