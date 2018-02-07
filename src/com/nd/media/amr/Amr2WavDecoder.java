package com.nd.media.amr;

/**
 * Created by kv.h on 14/11/21.
 */
public class Amr2WavDecoder {

    public static native int convert(String amrPath, String wavPath);

    static {
        System.loadLibrary("jsb-codec");
    }
}
