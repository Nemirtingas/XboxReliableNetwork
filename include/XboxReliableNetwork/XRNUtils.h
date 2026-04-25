#pragma once

uint32_t XRNGetTimestamp();
void XRNRandomBytes(void* buffer, size_t size);
uint16_t XRNCompressNCD(uint16_t packetId);
uint16_t XRNMapWireIdToPktId(uint16_t wNELWireId, uint16_t a2);