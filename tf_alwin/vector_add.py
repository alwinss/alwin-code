import tensorflow as tf

a = tf.constant([1.0,2.0],name="a")
b = tf.constant([1.0,2.0],name="b")
result = a+b
print(result)
sess = tf.Session()
sess.run(result)