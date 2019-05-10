var r = 1..10;            // Range
var d = {1..10};          // Domain
var d2 = {1..10, 1..10};  // 2 dimensional Domain
var A : [1..10] int;      // Array

var B : [{1..10}] int;    // equivalent to A
var C : [r] int;          // equivalent to A
var D : [d] int;          // equivalent to A
var E : [d2] int;         // access by E[x,y]
