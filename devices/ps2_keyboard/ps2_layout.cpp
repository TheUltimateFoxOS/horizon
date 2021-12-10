#include <ps2_layout.h>

#include <utils/abort.h>

using namespace ps2;

char ps2::keymap_de(uint8_t key, bool l_shift, bool r_shift, bool caps_lock) {
	if(l_shift || r_shift || caps_lock){
		switch(key){
			case 0x02: return('!'); break;
			case 0x03: return('"'); break;
			//case 0x04: return('§'); break;
			case 0x05: return('$'); break;
			case 0x06: return('%'); break;
			case 0x07: return('&'); break;
			case 0x08: return('/'); break;
			case 0x09: return('('); break;
			case 0x0A: return(')'); break;
			case 0x0B: return('='); break;
			case 0x0C: return('?'); break;
			case 0x0D: return('`'); break;
			case 0x10: return('Q'); break;
			case 0x11: return('W'); break;
			case 0x12: return('E'); break;
			case 0x13: return('R'); break;
			case 0x14: return('T'); break;
			case 0x15: return('Z'); break;
			case 0x16: return('U'); break;
			case 0x17: return('I'); break;
			case 0x18: return('O'); break;
			case 0x19: return('P'); break;
			case 0x1E: return('A'); break;
			case 0x1F: return('S'); break;
			case 0x20: return('D'); break;
			case 0x21: return('F'); break;
			case 0x22: return('G'); break;
			case 0x23: return('H'); break;
			case 0x24: return('J'); break;
			case 0x25: return('K'); break;
			case 0x26: return('L'); break;
			case 0x2C: return('Y'); break;
			case 0x2D: return('X'); break;
			case 0x2E: return('C'); break;
			case 0x2F: return('V'); break;
			case 0x30: return('B'); break;
			case 0x31: return('N'); break;
			case 0x32: return('M'); break;
			case 0x33: return(';'); break;
			case 0x34: return(':'); break;
			case 0x35: return('_'); break;
			case 0x1b: return('*'); break;
			case 0x1C: return('\n'); break;
			case 0x2b: return('\''); break;
			//case 0x29: return('°'); break;
			case 0x39: return(' '); break;
			default:
				break;
		}
	}
	else{
		switch(key){
			case 0x02: return('1'); break;
			case 0x03: return('2'); break;
			case 0x04: return('3'); break;
			case 0x05: return('4'); break;
			case 0x06: return('5'); break;
			case 0x07: return('6'); break;
			case 0x08: return('7'); break;
			case 0x09: return('8'); break;
			case 0x0A: return('9'); break;
			case 0x0B: return('0'); break;
			//case 0x0C: return('ß'); break;
			//case 0x0D: return('´'); break;
			case 0x0E: return('\b'); break;
			case 0x10: return('q'); break;
			case 0x11: return('w'); break;
			case 0x12: return('e'); break;
			case 0x13: return('r'); break;
			case 0x14: return('t'); break;
			case 0x15: return('z'); break;
			case 0x16: return('u'); break;
			case 0x17: return('i'); break;
			case 0x18: return('o'); break;
			case 0x19: return('p'); break;
			case 0x1E: return('a'); break;
			case 0x1F: return('s'); break;
			case 0x20: return('d'); break;
			case 0x21: return('f'); break;
			case 0x22: return('g'); break;
			case 0x23: return('h'); break;
			case 0x24: return('j'); break;
			case 0x25: return('k'); break;
			case 0x26: return('l'); break;
			case 0x2C: return('y'); break;
			case 0x2D: return('x'); break;
			case 0x2E: return('c'); break;
			case 0x2F: return('v'); break;
			case 0x30: return('b'); break;
			case 0x31: return('n'); break;
			case 0x32: return('m'); break;
			case 0x33: return(','); break;
			case 0x34: return('.'); break;
			case 0x35: return('-'); break;
			case 0x1b: return('+'); break;
			case 0x1C: return('\n'); break;
			case 0x2b: return('#'); break;
			case 0x29: return('^'); break;
			case 0x39: return(' '); break;
			default:
				break;
		}
	}
	return 0;
}

char ps2::keymap_us(uint8_t key, bool l_shift, bool r_shift, bool caps_lock){
	if(l_shift || r_shift || caps_lock){
		switch(key){
			case 0x02: return('!'); break;
			case 0x03: return('\"'); break;
			case 0x04: break; //Pound symbol
			case 0x05: return('$'); break;
			case 0x06: return('%'); break;
			case 0x07: return(94); break;
			case 0x08: return('&'); break;
			case 0x09: return('*'); break;
			case 0x0A: return('('); break;
			case 0x0B: return(')'); break;
			case 0x0C: return('_'); break;
			case 0x0D: return('+'); break;
			case 0x0E: return('\b'); break;
			case 0x10: return('Q'); break;
			case 0x11: return('W'); break;
			case 0x12: return('E'); break;
			case 0x13: return('R'); break;
			case 0x14: return('T'); break;
			case 0x15: return('Y'); break;
			case 0x16: return('U'); break;
			case 0x17: return('I'); break;
			case 0x18: return('O'); break;
			case 0x19: return('P'); break;
			case 0x1A: return('{'); break;
			case 0x1B: return('}'); break;
			case 0x1E: return('A'); break;
			case 0x1F: return('S'); break;
			case 0x20: return('D'); break;
			case 0x21: return('F'); break;
			case 0x22: return('G'); break;
			case 0x23: return('H'); break;
			case 0x24: return('J'); break;
			case 0x25: return('K'); break;
			case 0x26: return('L'); break;
			case 0x27: return(':'); break;
			case 0x28: return('@'); break;
			case 0x29: return(126); break;
			case 0x2B: return(124); break;
			case 0x2C: return('Z'); break;
			case 0x2D: return('X'); break;
			case 0x2E: return('C'); break;
			case 0x2F: return('V'); break;
			case 0x30: return('B'); break;
			case 0x31: return('N'); break;
			case 0x32: return('M'); break;
			case 0x33: return('<'); break;
			case 0x34: return('>'); break;
			case 0x35: return('?'); break;
			case 0x1C: return('\n'); break;
			case 0x39: return(' '); break;
			default:
				break;
		}
	}
	else{
		switch(key){
			case 0x02: return('1'); break;
			case 0x03: return('2'); break;
			case 0x04: return('3'); break;
			case 0x05: return('4'); break;
			case 0x06: return('5'); break;
			case 0x07: return('6'); break;
			case 0x08: return('7'); break;
			case 0x09: return('8'); break;
			case 0x0A: return('9'); break;
			case 0x0B: return('0'); break;
			case 0x0C: return('-'); break;
			case 0x0D: return('='); break;
			case 0x0E: return('\b'); break;
			case 0x10: return('q'); break;
			case 0x11: return('w'); break;
			case 0x12: return('e'); break;
			case 0x13: return('r'); break;
			case 0x14: return('t'); break;
			case 0x15: return('y'); break;
			case 0x16: return('u'); break;
			case 0x17: return('i'); break;
			case 0x18: return('o'); break;
			case 0x19: return('p'); break;
			case 0x1A: return('['); break;
			case 0x1B: return(']'); break;
			case 0x1C: return('\n'); break;
			case 0x1E: return('a'); break;
			case 0x1F: return('s'); break;
			case 0x20: return('d'); break;
			case 0x21: return('f'); break;
			case 0x22: return('g'); break;
			case 0x23: return('h'); break;
			case 0x24: return('j'); break;
			case 0x25: return('k'); break;
			case 0x26: return('l'); break;
			case 0x27: return(';'); break;
			case 0x28: return('\''); break;
			case 0x29: return('#'); break;
			case 0x2B: return('\\'); break;
			case 0x2C: return('z'); break;
			case 0x2D: return('x'); break;
			case 0x2E: return('c'); break;
			case 0x2F: return('v'); break;
			case 0x30: return('b'); break;
			case 0x31: return('n'); break;
			case 0x32: return('m'); break;
			case 0x33: return(','); break;
			case 0x34: return('.'); break;
			case 0x35: return('/'); break;
			case 0x39: return(' '); break;
			default:
				break;
		}
	}	
	return 0;
}

char ps2::keymap_fr(uint8_t key, bool l_shift, bool r_shift, bool caps_lock){
	if(l_shift || r_shift || caps_lock){
		switch(key){
			case 0x02: return('1'); break;
			case 0x03: return('2'); break;
			case 0x04: return('3'); break;
			case 0x05: return('4'); break;
			case 0x06: return('5'); break;
			case 0x07: return('6'); break;
			case 0x08: return('7'); break;
			case 0x09: return('8'); break;
			case 0x0A: return('9'); break;
			case 0x0B: return('0'); break;
			case 0x0C: return(176); break;
			case 0x0D: return('_'); break;
			case 0x0E: return('\b'); break;
			case 0x10: return('A'); break;
			case 0x11: return('Z'); break;
			case 0x12: return('E'); break;
			case 0x13: return('R'); break;
			case 0x14: return('T'); break;
			case 0x15: return('Y'); break;
			case 0x16: return('U'); break;
			case 0x17: return('I'); break;
			case 0x18: return('O'); break;
			case 0x19: return('P'); break;
			case 0x1A: return(168); break;
			case 0x1B: return(163); break;
			case 0x1E: return('Q'); break;
			case 0x1F: return('S'); break;
			case 0x20: return('D'); break;
			case 0x21: return('F'); break;
			case 0x22: return('G'); break;
			case 0x23: return('H'); break;
			case 0x24: return('J'); break;
			case 0x25: return('K'); break;
			case 0x26: return('L'); break;
			case 0x27: return('M'); break;
			case 0x28: return('%'); break;
			case 0x2C: return('W'); break;
			case 0x2D: return('X'); break;
			case 0x2E: return('C'); break;
			case 0x2F: return('V'); break;
			case 0x30: return('B'); break;
			case 0x31: return('N'); break;
			case 0x32: return('?'); break;
			case 0x33: return('.'); break;
			case 0x34: return('/'); break;
			case 0x35: return(167); break;
			case 0x1C: return('\n'); break;
			case 0x39: return(' '); break;
			case 0x56: return('>'); break;
			default:
				break;
		}
	}
	else{
		switch(key){
			case 0x02: return('&'); break;
			case 0x03: return(233); break;
			case 0x04: return('"'); break;
			case 0x05: return('\''); break;
			case 0x06: return('('); break;
			case 0x07: return('-'); break;
			case 0x08: return(232); break;
			case 0x09: return('_'); break;
			case 0x0A: return(231); break;
			case 0x0B: return(224); break;
			case 0x0C: return(')'); break;
			case 0x0D: return('='); break;
			case 0x0E: return('\b'); break;
			case 0x10: return('a'); break;
			case 0x11: return('z'); break;
			case 0x12: return('e'); break;
			case 0x13: return('r'); break;
			case 0x14: return('t'); break;
			case 0x15: return('y'); break;
			case 0x16: return('u'); break;
			case 0x17: return('i'); break;
			case 0x18: return('o'); break;
			case 0x19: return('p'); break;
			case 0x1A: return('^'); break;
			case 0x1B: return('$'); break;
			case 0x1E: return('q'); break;
			case 0x1F: return('s'); break;
			case 0x20: return('d'); break;
			case 0x21: return('f'); break;
			case 0x22: return('g'); break;
			case 0x23: return('h'); break;
			case 0x24: return('j'); break;
			case 0x25: return('k'); break;
			case 0x26: return('l'); break;
			case 0x27: return('m'); break;
			case 0x28: return(249); break;
			case 0x29: return('*'); break;
			case 0x2C: return('w'); break;
			case 0x2D: return('x'); break;
			case 0x2E: return('c'); break;
			case 0x2F: return('v'); break;
			case 0x30: return('b'); break;
			case 0x31: return('n'); break;
			case 0x32: return(','); break;
			case 0x33: return(';'); break;
			case 0x34: return(':'); break;
			case 0x35: return('!'); break;
			case 0x1C: return('\n'); break;
			case 0x39: return(' '); break;
			case 0x56: return('<'); break;
			default:
				break;
		}
	}
	return 0;
}

char ps2::keymap(keymap_layout keymap_id, uint8_t key, bool l_shift, bool r_shift, bool caps_lock) {
	switch (keymap_id) {
		case keymap_layout::keymap_de_e:
			return keymap_de(key, l_shift, r_shift, caps_lock);
		case keymap_layout::keymap_us_e:
			return keymap_us(key, l_shift, r_shift, caps_lock);
		case keymap_layout::keymap_fr_e:
			return keymap_fr(key, l_shift, r_shift, caps_lock);
		default:
			abortf("Invalid keymap_layout %d selected!", keymap_id);
	}
}