package top.bufeiniao.yuban.model.data;

public class DatumRecord {
    private BaseDataType baseDataType;
    private Object child;
    private int address;
    private int size;
    private boolean isDynamicArray;
    private DatumRecord(BaseDataType baseDataType, Object datum, int address, int size, boolean isDynamicArray){
        this.baseDataType=baseDataType;
        this.child =datum;
        this.address=address;
        this.size=size;
        this.isDynamicArray=isDynamicArray;
    }

    public BaseDataType getBaseDataType() {
        return baseDataType;
    }

    public void setBaseDataType(BaseDataType baseDataType) {
        this.baseDataType = baseDataType;
    }

    public Object getChild() {
        return child;
    }

    public void setChild(Object child) {
        this.child = child;
    }

    public int getAddress() {
        return address;
    }

    public void setAddress(int address) {
        this.address = address;
    }

    public int getSize() {
        return size;
    }

    public void setSize(int size) {
        this.size = size;
    }

    public boolean isDynamicArray() {
        return isDynamicArray;
    }

    public void setDynamicArray(boolean dynamicArray) {
        isDynamicArray = dynamicArray;
    }


    public static class Builder{
        private final BaseDataType baseDataType;
        private final Object datum;
        private int address=0;
        private int size=-1;
        private boolean isDynamicArray=false;
        private Builder(BaseDataType baseDataType,Object datum){
            this.baseDataType=baseDataType;
            this.datum=datum;
        }
        public Builder setAddress(int address){
            this.address=address;
            return this;
        }
        public Builder setSize(int size){
            this.size=size;
            return this;
        }
        public Builder isDynamicArray(boolean isDynamicArray){
            this.isDynamicArray=isDynamicArray;
            return this;
        }
        public DatumRecord build(){
            return new DatumRecord(baseDataType,datum,address,size,isDynamicArray);
        }
    }

}
