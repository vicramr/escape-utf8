// This is an exception class thrown by StreamPair to indicate an issue with opening files.

class StreamPairException extends Exception {
    StreamPairException(String message) {
        super(message);
    }
}
