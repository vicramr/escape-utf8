// This is an exception class thrown by StreamPair to indicate an issue with opening files.

class StreamPairException extends Exception {
    // See docs for why we need serialVersionUID:
    // https://docs.oracle.com/javase/8/docs/api/java/io/Serializable.html
    private static final long serialVersionUID = 1L;

    StreamPairException(String message) {
        super(message);
    }
}
