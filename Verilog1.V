module pwm(clk,D1,D2,sw,p1,p2,led);
   input clk;
   input [7:0] D1;
   input [7:0] D2;
   output reg p1  = 1'b1;
   output reg p2  = 1'b1;
   output reg sw = 1'b1;
   output reg [7:0] led;

   parameter freq  = 10000;     //required pulse frequency
   parameter period_count = 50000000/(freq*100);

   reg [7:0] duty1;
   reg [7:0] duty2;
   reg [15:0] on_count1;
   reg [15:0] on_count2;
   integer count1 = 0,count2 = 0;

   always @(posedge clk)
   begin
      count1 = count1 + 1;
      if(count1 <= on_count1)
           p1 = 1'b1;
      else if(count1 <= period_count*100)
           p1 = 1'b0;
      else
      begin
           count1 = 1;
           p1 = 1'b1;
      end


   end

   always @(posedge clk)
   begin
      count2 = count2 + 1;
      if(count2 <= on_count2)
           p2 = 1'b1;
      else if(count2 <= period_count*100)
           p2 = 1'b0;
      else
      begin
           count2 = 1;
           p2 = 1'b1;
      end
   end

   always @(D1)
   begin
        duty1 = D1;
        on_count1 = duty1 * period_count;
   end

   always @(D2)
   begin
        if(D2 == 0)
            sw = 1'b0;
        else
            sw = 1'b1;
            
        duty2 = D2;
        on_count2 = duty2 * period_count;
   end

endmodule
