const {TextGps} = require("bindings")('search')
import {SqliteCode} from "./status"

export type Points = {
    start:number;
    end:number;
}

export type Result = {
    contents:string[][],
    locs:Array<Array<Points>>
}

export class TextGPS {
    private dbFile:string;
    private gps:any;
    constructor() {
        this.dbFile = ''
        this.gps = new TextGps();
    }

    open(path:string):void {
        const rc = this.gps.open(path);
        this.dbFile = path;
        if (rc != 0) {
            const error = new Error(SqliteCode[rc]);
            console.error(error.message,error.stack);
        }
    }

    close():void {
        const rc = this.gps.close();
        if (rc != 0) {
            const error = new Error(SqliteCode[rc]);
            console.error(error.message,error.stack);
        }
    }

    commit():void {
        const rc = this.gps.commit();
        if (rc != 0) {
            const error = new Error(SqliteCode[rc]);
            console.error(error.message,error.stack);
        }
    }

    execute(sql:string,bindings?:string[][]):void {
        const rc = this.gps.execute(sql,bindings);
        if (rc != 0) {
            const error = new Error(SqliteCode[rc]);
            console.error(error.message,error.stack);
        }
    }

    stopSearching(stop:boolean):void {
        this.gps.stopSearching(stop);
    }

    setCase(value:boolean):void {
        this.gps.setCase(value);
    }

    setTop(top:number):void {
        this.gps.setTop(top);
    }

    sortByScoreAscent():void {
        this.gps.sortByScoreAscent();
    }

    sortByScoreDescent():void {
        this.gps.sortByScoreDescent();
    }

    sortBySizeAscent():void {
        this.gps.sortBySizeAscent();
    }

    sortBySizeDescent():void {
        this.gps.sortBySizeDescent();
    }

    sortByCTimeAscent():void {
        this.gps.sortByCTimeAscent();
    }

    sortByCTimeDescent():void {
        this.gps.sortByCTimeDescent();
    }

    sortByMTimeAscent():void {
        this.gps.sortByMTimeAscent();
    }

    sortByMTimeDescent():void {
        this.gps.sortByMTimeDescent();
    }

    sortByTitleAscent():void {
        this.gps.sortByTitleAscent();
    }

    sortByTitleDescent():void {
        this.gps.sortByTitleDescent();
    }

    sortByFormatAscent():void {
        this.gps.sortByFormatAscent();
    }

    sortByFormatDescent():void {
        this.gps.sortByFormatDescent();
    }

    sortByPathAscent():void {
        this.gps.sortByPathAscent();
    }

    sortByPathDescent():void {
        this.gps.sortByPathDescent();
    }

    search(queries:string[],logics?:string[]) {
        this.gps.search(queries,logics);
    }

    getResult():Result {
        return this.gps.getResult();
    }

    getDbPath():string {
        return this.dbFile;
    }
}