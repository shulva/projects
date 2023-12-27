module top_module ( 
    input a, 
    input b, 
    input c,
    input d,
    output out1,
    output out2
);
    mod_a instance1(.in1(a),.out1(out1),.out2(out2),.in4(d),.in3(c),.in2(b));

endmodule
