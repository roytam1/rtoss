BEGIN TRANSACTION;
CREATE TABLE "Fav"("id" INTEGER  NOT NULL PRIMARY KEY,"cat" bit(1),"name" varchar(255),"addr" varchar(255),"catid" INTEGER,"protected" bit(1),"ord" INTEGER DEFAULT '0' NULL);
COMMIT;
