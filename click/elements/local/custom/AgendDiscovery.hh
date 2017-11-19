//extends ICMP Router Discovery
//is formed by including an extension
//IP TTL must be 1
//used to determine current point of attachment
/*
other fields:
link layer destination address: same as source link-layer address 
IP destination: 255.255.255.255
ICMP fields: 
	code: 0 or 16
	lifetime
	router addresses
	num addrs

interval in which these advertisments are send shouldn't be longer than 1/3 of the lifetime given in the ICMP header
*/
