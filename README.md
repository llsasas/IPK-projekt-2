# Documentation - IPK 2. Project
## Samuel Čus (xcussa00)

### Content
1. Content
2. Theory
3. Testing
4. References

### Theory
This program is a server implementation for the IPK Calculator Protocol. It allows server to communicate with clients using IPKCPD. It supports both TCP and UDP. What they have in common, what are the differences between these two protocols and how they work is explained below. 

**Main differences**
TCP is a connection-oriented protocol, which means it establishes a connection between two devices before transferring data. UDP, on the other hand, is a connectionless protocol and does not establish a dedicated end-to-end connection. TCP is a reliable protocol, meaning it guarantees that all data packets will be delivered to the destination device in the correct order without loss or duplication. UDP is an unreliable protocol and does not guarantee delivery, ordering or duplicate protection. Another difference is that TCP is commonly used for applications that require reliable and ordered data transfer, such as file transfer and email. UDP is commonly used for applications that require real-time, low-latency data transfer, such as online gaming, video conferencing and VoIP.

**TCP**
TCP (Transmission Control Protocol)  establishes a connection between two devices, divides data into packets, assigns a sequence number to each packet, and verifies packet integrity. It guarantees reliable and ordered data delivery by retransmitting lost packets and managing flow control.

**UDP**
UDP (User Datagram Protocol) is a connectionless protocol that does not establish a virtual circuit between devices before transmitting data. It simply sends data packets to the destination device without verifying if it is ready to receive them or not. UDP does not guarantee reliable data delivery or packet ordering, as it does not have a mechanism to check for packet loss or duplicates. It also does not have flow control mechanisms to manage the rate of data transfer.


### Testing
The testing of the programme was mainly carried out by manual tests. Giving programme inputs that should return some errors and checking if it did return them. Then trying various valid combinations and checking if the programme will return without any errors. It was not specified in the assignment whether these test should be manual or automatic so I decided for the manual ones. Few examples of the tests I did are in /doc_testing.

### References 

**Handling ctrlc**
Title: Catch Ctrl-C in C  
Author: Various contributors  
Website: Stack Overflow  
URL: https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c

**How to solve prefix expression**
Title: Evaluation of Prefix Expressions  
Author: GeeksforGeeks contributors  
Website: GeeksforGeeks  
URL: https://www.geeksforgeeks.org/evaluation-prefix-expressions/
 
