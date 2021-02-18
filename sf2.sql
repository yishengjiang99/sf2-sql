

create table phdr(
	id int,
	name varchar(20),
	bank_id smallint unsigned,
	pbag_id smallint unsigned
);
create	unique index id_bank_id on phdr (id, bank_id); 


create table pbag(
	id int,
	pgen_id smallint unsigned,
	pmod_id smallint unsigned
);
create index pbg on pbag(pgen_id);
create index pbm on pbag(pmod_id);

create table pmod (
	id int,
	mod_src smallint unsigned,
	mod_dest smallint unsigned,
	mod_amt smallint unsigned,
	mod_amt_src smallint unsigned,
	transpose smallint unsigned,
	index(id)
);

create table pgen(id int, operator smallint unsigned, lo smallint, hi smallint, index(id));

create table inst(id int, name varchar(20),  ibag_id smallint unsigned, index(id));

create table ibag(id int, igen_id smallint unsigned, imod_id smallint unsigned);
create table imod(
	id int, 	
	mod_src smallint unsigned,
	mod_dest smallint unsigned,
	mod_amt smallint unsigned,
	mod_amt_src smallint unsigned,
	transpose smallint unsigned
	);

create table igen(id int, operator smallint unsigned, lo smallint, hi smallint);

create table shdr(id int, name char(20), start int unsigned, end int unsigned, startLoop int unsigned, endLoop int unsigned, sampleRate int unsigned,
originalPitch smallint unsigned, pitchCorrection smallint, sampleLink mediumint unsigned, sampleType mediumint unsigned);
