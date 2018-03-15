# counter
C++ wrapper for counters that can roll-over (e.g. timestamps/ack-ids)

Represents an unsigned counter that can roll over from its maximum value
back to zero.  A common example is a 32-bit timestamp from GetTickCount()
on Windows, which can roll-over and cause software bugs despite testing.

This class provides:
+ Counters of 2-64 bits e.g. 24-bit counters
+ Increment/decrement by 1 or a constant
+ Safe comparison operator overloads
+ Compression/decompression via truncation
+ Unit tested software

What problems does this solve?


### (Solution 1): It solves wrap-around issues commonly seen in code involving timestamps.

Example buggy code:

    //DWORD t = ::GetTickCount();
    DWORD t = 0xffffff00;
    DWORD kTimeout = t + 300; // Wait for 300 ms
	// kTimeout == 0x0000002c here.

    while (t < kTimeout)
    {
		//::Sleep(10);
		//t = ::GetTickCount();
		t += 10;
    }

This will not wait the full 300 ms, because the `(t < kTimeout)` will be false: `(0xffffff00 < 0x0000002c)`.
Instead the exact same code can be written with Counter32 and it will work:

    //Counter32 t = ::GetTickCount();
    Counter32 t = 0xffffff00;
    Counter32 kTimeout = t + 300; // Wait for 300 ms
	// kTimeout == 0x0000002c here.

    while (t < kTimeout)
    {
		//::Sleep(10);
		//t = ::GetTickCount();
		t += 10;
    }


### (Solution 2): When transporting counters (timestamps, ack-ids) across the Internet using UDP/IP packets, timestamps do not change much between messages so they can be compressed.


Example usage:

    // Compress a counter for transmission over the Internet:

    Counter16 t1_compressed;
    t1.Compress(t1_compressed);

    // Now decompress it:

    Counter32 t1_decompressed;
    t1_compressed.Decompress(t0, t1_decompressed);


The compression works so long as the timestamp only wraps around once and not twice.


### (Solution 3): Allows for counters that are not the same size as a basic type (e.g. 24 bits).

Example usage:

	// A few predefined types are available:
	Counter24 x;
	++x;

	// But you can also write your own unusually sized counter:
	Counter<uint64_t, 57> my_57_bit_counter;
	++my_57_bit_counter;
