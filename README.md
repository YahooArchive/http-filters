Filters for HTTP requests

![Diagram](https://github.com/yahoo/http-filters/raw/master/diagram.png)

Structure
```
Forest f;

{
  Tree t;
  t.addAnd();
    CHILD_OP(t, "isMethod", "GET");
    OP(t, "isScheme", "http");
    t.parent();
  f.push_back(t);
}

{
  Tree t;
  t.addOr();
    t.addChildAnd();
      CHILD_OP(t, "existsHeader", "User-Agent");
      OP(t, "containsHeader", "User-Agent", "Firefox");
      t.parent();
    t.addAnd();
      CHILD_OP(t, "existsHeader", "user-agent");
      OP(t, "containsHeader", "user-agent", "Firefox");
      t.parent();
    t.parent();
  f.push_back(t);
}

{
  Tree t;
  OP(t, "containsDomain", ".yahoo.com");
  f.push_back(t);
}

{
  Tree t;
  OP(t, "equalPath", "search");
  f.push_back(t);
}

{
  Tree t;
  OP(t, "containsQueryParameter", "state", "california");
  f.push_back(t);
}
```

VM Code
```
printing vm code
     0 0x000000 kSkip                         0x0       0x0       0x0

 -- Entry 1 --
     1 0x100000 kIsMethod                     0x1       0x3       0x0
 -> "GET"
     2 0x200000 kIsScheme                     0x5       0x4       0x0
 -> "http"
     3 0x300000 kReturn                       0x0       0x0       0x0
     4 0x400000 kExecute                      0x2       0x1       0x0        // kAnd, Entry 1
     5 0x500000 kReturn                       0x0       0x0       0x0
     6 0x600000 kHalt                         0x0       0x0       0x0

 -- Entry 2 --
     7 0x700000 kExistsHeader                 0xa       0x0       0x0
 -> "User-Agent"
     8 0x800000 kContainsHeader               0xa       0x15      0x0
 -> "User-Agent"
 -> "Firefox"
     9 0x900000 kReturn                       0x0       0x0       0x0

 -- Entry 3 --
    10 0xa00000 kExistsHeader                 0x1d      0x0       0x0
 -> "user-agent"
    11 0xb00000 kContainsHeader               0x1d      0x15      0x0
 -> "user-agent"
 -> "Firefox"
    12 0xc00000 kReturn                       0x0       0x0       0x0

 -- Entry 4 --
    13 0xd00000 kExecute                      0x2       0x7       0x0        // kAnd, Entry 2
    14 0xe00000 kExecute                      0x2       0xa       0x0        // kAnd, Entry 3
    15 0xf00000 kReturn                       0x0       0x0       0x0
    16 0x100000 kExecute                      0x3       0xd       0x0        // kOr, Entry 4
    17 0x110000 kReturn                       0x0       0x0       0x0
    18 0x120000 kHalt                         0x0       0x0       0x0
    19 0x130000 kContainsDomain               0x28      0xa       0x0
 -> ".yahoo.com"
    20 0x140000 kReturn                       0x0       0x0       0x0
    21 0x150000 kHalt                         0x0       0x0       0x0
    22 0x160000 kEqualPath                    0x33      0x6       0x0
 -> "search"
    23 0x170000 kReturn                       0x0       0x0       0x0
    24 0x180000 kHalt                         0x0       0x0       0x0
    25 0x190000 kContainsQueryParameter       0x3a      0x40      0x0
 -> "state"
 -> "california"
    26 0x1a0000 kReturn                       0x0       0x0       0x0
    27 0x1b0000 kHalt                         0x0       0x0       0x0
```
