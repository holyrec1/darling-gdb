/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef N
#error "N must be #defined"
#endif

#include "sim-xcat.h"

/* NOTE: see end of file for #undef of these macros */
#define unsigned_N XCONCAT2(unsigned_,N)
#define T2H_N XCONCAT2(T2H_,N)
#define H2T_N XCONCAT2(H2T_,N)

#define sim_core_read_aligned_N XCONCAT2(sim_core_read_aligned_,N)
#define sim_core_write_aligned_N XCONCAT2(sim_core_write_aligned_,N)
#define sim_core_read_unaligned_N XCONCAT2(sim_core_read_unaligned_,N)
#define sim_core_write_unaligned_N XCONCAT2(sim_core_write_unaligned_,N)

/* TAGS: sim_core_read_aligned_1 sim_core_read_aligned_2 */
/* TAGS: sim_core_read_aligned_4 sim_core_read_aligned_8 */
/* TAGS: sim_core_read_aligned_word */

INLINE_SIM_CORE(unsigned_N)
sim_core_read_aligned_N(sim_cpu *cpu,
			sim_cia cia,
			sim_core_maps map,
			unsigned_word xaddr)
{
  sim_cpu_core *cpu_core = CPU_CORE (cpu);
  sim_core_common *core = &cpu_core->common;
  unsigned_N val;
  sim_core_mapping *mapping;
  address_word addr;
  if (WITH_XOR_ENDIAN)
    addr = xaddr ^ cpu_core->xor[(sizeof(unsigned_N) - 1) % WITH_XOR_ENDIAN];
  else
    addr = xaddr;
  mapping = sim_core_find_mapping (core, map,
				   addr,
				   sizeof (unsigned_N),
				   read_transfer,
				   1 /*abort*/, cpu, cia);
#if (WITH_DEVICES)
  if (WITH_CALLBACK_MEMORY && mapping->device != NULL) {
    unsigned_N data;
    if (device_io_read_buffer (mapping->device,
			       &data,
			       mapping->space,
			       addr,
			       sizeof (unsigned_N)) != sizeof (unsigned_N))
      device_error (mapping->device, "internal error - %s - io_read_buffer should not fail",
		    XSTRING (sim_core_read_aligned_N));
    val = T2H_N (data);
  }
  else
#endif
    val = T2H_N (*(unsigned_N*) sim_core_translate (mapping, addr));
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    if (sizeof (unsigned_N) > 4)
      trace_printf (CPU_STATE (cpu), cpu,
		    "sim-n-core.c:%d: read-%d %s:0x%08lx -> 0x%08lx%08lx\n",
		    __LINE__,
		    sizeof (unsigned_N),
		    sim_core_map_to_str (map),
		    (unsigned long) addr,
		    (unsigned long) (((unsigned64)(val)) >> 32),
		    (unsigned long) val);
    else
      trace_printf (CPU_STATE (cpu), cpu,
		    "sim-n-core.c:%d: read-%d %s:0x%08lx -> 0x%0*lx\n",
		    __LINE__,
		    sizeof (unsigned_N),
		    sim_core_map_to_str (map),
		    (unsigned long) addr,
		    sizeof (unsigned_N) * 2,
		    (unsigned long) val);
  return val;
}

/* TAGS: sim_core_read_unaligned_1 sim_core_read_unaligned_2 */
/* TAGS: sim_core_read_unaligned_4 sim_core_read_unaligned_8 */
/* TAGS: sim_core_read_unaligned_word */

INLINE_SIM_CORE(unsigned_N)
sim_core_read_unaligned_N(sim_cpu *cpu,
			  sim_cia cia,
			  sim_core_maps map,
			  address_word addr)
{
  int alignment = sizeof (unsigned_N) - 1;
  /* if hardwired to forced alignment just do it */
  if (WITH_ALIGNMENT == FORCED_ALIGNMENT)
    return sim_core_read_aligned_N (cpu, cia, map, addr & ~alignment);
  else if ((addr & alignment) == 0)
    return sim_core_read_aligned_N (cpu, cia, map, addr);
  else
    switch (CURRENT_ALIGNMENT)
      {
      case STRICT_ALIGNMENT:
	SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map,
			 sizeof (unsigned_N), addr,
			 read_transfer, sim_core_unaligned_signal);
	return -1;
      case NONSTRICT_ALIGNMENT:
	{
	  unsigned_N val;
	  if (sim_core_xor_read_buffer (CPU_STATE (cpu), cpu, map, &val, addr,
					sizeof(unsigned_N))
	      != sizeof(unsigned_N))
	    SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map,
			     sizeof (unsigned_N), addr,
			     read_transfer, sim_core_unaligned_signal);
	  val = T2H_N(val);
	  return val;
	}
      case FORCED_ALIGNMENT:
	return sim_core_read_aligned_N (cpu, cia, map, addr & ~alignment);
      case MIXED_ALIGNMENT:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - mixed alignment",
			  XSTRING (sim_core_read_unaligned_N));
	return 0;
      default:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - bad switch",
			  XSTRING (sim_core_read_unaligned_N));
	return 0;
      }
}

/* TAGS: sim_core_write_aligned_1 sim_core_write_aligned_2 */
/* TAGS: sim_core_write_aligned_4 sim_core_write_aligned_8 */
/* TAGS: sim_core_write_aligned_word */

INLINE_SIM_CORE(void)
sim_core_write_aligned_N(sim_cpu *cpu,
			 sim_cia cia,
			 sim_core_maps map,
			 unsigned_word xaddr,
			 unsigned_N val)
{
  sim_cpu_core *cpu_core = CPU_CORE (cpu);
  sim_core_common *core = &cpu_core->common;
  sim_core_mapping *mapping;
  address_word addr;
  if (WITH_XOR_ENDIAN)
    addr = xaddr ^ cpu_core->xor[(sizeof(unsigned_N) - 1) % WITH_XOR_ENDIAN];
  else
    addr = xaddr;
  mapping = sim_core_find_mapping(core, map,
				  addr,
				  sizeof (unsigned_N),
				  write_transfer,
				  1 /*abort*/, cpu, cia);
#if (WITH_DEVICES)
  if (WITH_CALLBACK_MEMORY && mapping->device != NULL) {
    unsigned_N data = H2T_N (val);
    if (device_io_write_buffer (mapping->device,
				&data,
				mapping->space,
				addr,
				sizeof (unsigned_N), /* nr_bytes */
				cpu,
				cia) != sizeof (unsigned_N))
      device_error (mapping->device, "internal error - %s - io_write_buffer should not fail",
		    XSTRING (sim_core_write_aligned_N));
  }
  else
#endif
    *(unsigned_N*) sim_core_translate (mapping, addr) = H2T_N (val);
  if (TRACE_P (cpu, TRACE_CORE_IDX))
    if (sizeof (unsigned_N) > 4)
      trace_printf (CPU_STATE (cpu), cpu,
		    "sim-n-core.c:%d: write-%d %s:0x%08lx <- 0x%08lx%08lx\n",
		    __LINE__,
		    sizeof (unsigned_N),
		    sim_core_map_to_str (map),
		    (unsigned long) addr,
		    (unsigned long) (((unsigned64)(val)) >> 32),
		    (unsigned long) val);
    else
      trace_printf (CPU_STATE (cpu), cpu,
		    "sim-n-core.c:%d: write-%d %s:0x%08lx <- 0x%0*lx\n",
		    __LINE__,
		    sizeof (unsigned_N),
		    sim_core_map_to_str (map),
		    (unsigned long) addr,
		    sizeof (unsigned_N) * 2,
		    (unsigned long) val);
}

/* TAGS: sim_core_write_unaligned_1 sim_core_write_unaligned_2 */
/* TAGS: sim_core_write_unaligned_4 sim_core_write_unaligned_8 */
/* TAGS: sim_core_write_unaligned_word */

INLINE_SIM_CORE(void)
sim_core_write_unaligned_N(sim_cpu *cpu,
			   sim_cia cia,
			   sim_core_maps map,
			   address_word addr,
			   unsigned_N val)
{
  int alignment = sizeof (unsigned_N) - 1;
  /* if hardwired to forced alignment just do it */
  if (WITH_ALIGNMENT == FORCED_ALIGNMENT)
    sim_core_write_aligned_N (cpu, cia, map, addr & ~alignment, val);
  else if ((addr & alignment) == 0)
    sim_core_write_aligned_N (cpu, cia, map, addr, val);
  else
    switch (CURRENT_ALIGNMENT)
      {
      case STRICT_ALIGNMENT:
	SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map,
			 sizeof (unsigned_N), addr,
			 write_transfer, sim_core_unaligned_signal);
	break;
      case NONSTRICT_ALIGNMENT:
	{
	  unsigned_N val = H2T_N (val);
	  if (sim_core_xor_write_buffer (CPU_STATE (cpu), cpu, map, &val, addr,
					 sizeof(unsigned_N))
	      != sizeof(unsigned_N))
	    SIM_CORE_SIGNAL (CPU_STATE (cpu), cpu, cia, map,
			     sizeof (unsigned_N), addr,
			     write_transfer, sim_core_unaligned_signal);
	  break;
	}
      case FORCED_ALIGNMENT:
	sim_core_write_aligned_N (cpu, cia, map, addr & ~alignment, val);
	break;
      case MIXED_ALIGNMENT:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - mixed alignment",
			  XSTRING (sim_core_write_unaligned_N));
	break;
      default:
	sim_engine_abort (CPU_STATE (cpu), cpu, cia,
			  "internal error - %s - bad switch",
			  XSTRING (sim_core_write_unaligned_N));
	break;
      }
}


/* NOTE: see start of file for #define of these macros */
#undef unsigned_N
#undef T2H_N
#undef H2T_N
#undef sim_core_read_aligned_N
#undef sim_core_write_aligned_N
#undef sim_core_read_unaligned_N
#undef sim_core_write_unaligned_N
