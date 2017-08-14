/*
 * A sample, extra-simple block driver. Updated for kernel 2.6.31.
 *
 * (C) 2003 Eklektix, Inc.
 * (C) 2010 Pat Patterson <pat at superpat dot com>
 * Redistributable under the terms of the GNU GPL.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */ 
#include <linux/fs.h>     /* everything... */ 
#include <linux/errno.h>  /* error codes */ 
#include <linux/types.h>  /* size_t */ 
#include <linux/vmalloc.h>/* vmalloc */ 
#include <linux/genhd.h> 
#include <linux/blkdev.h> 
#include <linux/hdreg.h>  /* HDIO_GETGEO partitioning*/
#include <linux/slab.h>	  /* kmmalloc */
#include <linux/fcntl.h>  /* File I/O */

#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/kdev_t.h>
#include <linux/buffer_head.h>
#include <linux/bio.h>

#include <linux/crypto.h>
#include <linux/scatterlist.h>

MODULE_LICENSE("GPL");

/* Device Major Number */
static int major_num = 0;
module_param(major_num, int, 0);

/* Sector Size */
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);

/* # of Sectors (~524 kiB) */
static int nsectors = 1024; /* How big the drive is */
module_param(nsectors, int, 0);

/*
 * We can tweak our hardware sector size, but the kernel talks to us
 * in terms of small sectors, always.
 */
#define KERNEL_SECTOR_SIZE 512

/*
 * Our request queue.
 */
static struct request_queue *Queue;

/*
 * Device data
 */
static struct sbd_device {
	unsigned long size;	/* Device size (sectors) */
	spinlock_t lock;	/* Mutual Exclusion */
	u8 *data;		/* Data Array */
	struct gendisk *gd;     /* Gendisk structure */
} Device;

// The crypto key is required to be 16 bytes long. 
struct crypto_cipher *tfm;
static char *key = "1234567890123456";
module_param(key, charp, 0);
static int keylen = 16;
module_param(keylen, int, 0);

/*
 * Handle an I/O request.
 */
static void sbd_transfer(struct sbd_device *dev, sector_t sector,
		unsigned long nsect, char *buffer, int write) {
	/* param: (nsect) : # of sectors for reading/writing,
	 * param: (sector): Starting sector */
	unsigned long offset = sector * logical_block_size;
	unsigned long nbytes = nsect * logical_block_size;
	
	if (write)
		printk("Test: [Transfer] Transfering data for WRITE\n");
	else
		printk("Test: [Transfer] Transfering data for READ\n");
	
	if ((offset + nbytes) > dev->size) {
		printk("Test: [Transfer] too far offset: %ld nbytes: %ld\n", offset, nbytes);
		return;
	}
	
	if (crypto_cipher_setkey(tfm, key, keylen) == 0) {
		printk("Test: [Transfer] Setted crypto key\n");
	} else {
		printk("Test: [Transfer] Failed to set crypto key\n");
	}
	
	/* 
	 * For writing: Transfer data from buffer to device data
	 * For reading: Transfer data from device data to buffer 
	 * Encrypts/Descrypts transfered data one block at a time
	 * until it reaches nbytes long
	 */
	
	int i;
 
	if (write) {
		//memcpy(dev->data + offset, buffer, nbytes);
		
		printk("Test: [Transfer] Writing %lu bytes into device data\n", nbytes);
		
		for (i = 0; i < nbytes; i += crypto_cipher_blocksize(tfm)) {
			/* 
			 * Using tfm, the cipher handler to encrypt data from src to dest
			 * Encrypts data that is at least 1 block in size
			 */
			crypto_cipher_encrypt_one(
					tfm,	 				/* Cipher handler */
					dev->data + offset + i,	/* Dest */
					buffer + i				/* Src */
					);
		}
	}
	else {
		//memcpy(buffer, dev->data + offset, nbytes);
		printk("Test: [Transfer] Reading %lu bytes from device data\n", nbytes);
		
		for (i = 0; i < nbytes; i += crypto_cipher_blocksize(tfm)) {
			crypto_cipher_decrypt_one(
					tfm,					/* Cipher handler */
					buffer + i,				/* Dest */
					dev->data + offset + i	/* Src */
					);
		}
	}
	
	printk("Test: [Transfer] Completed Transfer and encryption\n");
}

static void sbd_request(struct request_queue *q) {
	struct request *req;

	/* Fetches a request at the top of the queue */
	req = blk_fetch_request(q);
	
	printk("Test: [Request] Getting Requests\n");
	
	/* Iterate through requests in queue */
	while (req != NULL) {
		
		/* Skip all non-command request */
		if (req == NULL || (req->cmd_type != REQ_TYPE_FS)) {
			printk ("Test: [Request] Skipped non-CMD request\n");
			__blk_end_request_all(req, -EIO);
			continue;
		}
		
		sbd_transfer(&Device, 				/*Device Data (*dev)*/
					blk_rq_pos(req),		/*Request Sector (sector)*/
					blk_rq_cur_sectors(req),/*# of sectors (nsect)*/
					req->buffer, 			/*buffer (buffer)*/
					rq_data_dir(req)); 		/*(write)*/
		
		printk("Test: [Request] Transfered data in Request\n");
		
		/* Fetches the next request */
		if ( ! __blk_end_request_cur(req, 0) ) {
			req = blk_fetch_request(q);
		}
		
		printk("Test: [Request] Completed all requests\n");
	}
}

/*
 * The HDIO_GETGEO ioctl is handled in blkdev_ioctl(), which
 * calls this. We need to implement getgeo, since we can't
 * use tools such as fdisk to partition the drive otherwise.
 */
int sbd_getgeo(struct block_device * block_device, struct hd_geometry * geo) {
	long size;

	printk("Test: [Getgeo] Partitioning.\n");
	
	/* We have no real geometry, of course, so make something up. */
	size = Device.size * (logical_block_size / KERNEL_SECTOR_SIZE);
	geo->cylinders = (size & ~0x3f) >> 6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 0;
	
	printk("Test: [Getgeo] Completed Partitioning.\n");
	
	return 0;
}

/*
 * The device operations structure.
 */
static struct block_device_operations sbd_ops = {
		.owner  = THIS_MODULE,
		.getgeo = sbd_getgeo
};

static int __init sbd_init(void) {
	/* File I/O */
	mm_segment_t oldfs;
	struct file *filp = NULL;
	unsigned long long offset = 0;
	ssize_t size;
	
	printk("Test: [INIT] Initializing\n");
	
	/* Registering block device */
	major_num = register_blkdev(major_num, "sbd");
	
	if (major_num < 0) {
		printk("Test: [INIT] Failed to register block device\n");
		
		/* Device or resource busy */
		return -EBUSY;
	}
	
	/* Set up the device, set Device to all 0*/
	memset(&Device, 0, sizeof(struct sbd_device));
	Device.size = nsectors * logical_block_size;
	Device.data = vmalloc(Device.size);

	memset(Device.data, 0, Device.size);

	/* Check if Device data is allocated */
	if (Device.data == NULL) {
		printk("Test: [INIT] Failed to allocate Device.data\n");
		unregister_blkdev(major_num, "sbd");
		return -ENOMEM;
	}
	
	printk("Test: [INIT] Device Size: %ld\n", Device.size);
	
	/* Now copy the device data from a file. If it doesn't exist, create 
	 * it. */
	oldfs = get_fs();
	set_fs(get_ds());
	filp = filp_open("/Data", O_RDONLY | O_CREAT, S_IRWXUGO);

	printk("Test: [INIT] attempted to open /Data file\n");
	
	/* Check for any errors or NULL from reading */
	if (IS_ERR(filp)) {
		printk("Test: [INIT] Failed to open file /Data file\n");
		set_fs(oldfs);
	} else {
		/* Read File */
		size = vfs_read(filp, Device.data, Device.size, &offset);
		printk("Test: [INIT] File output: size: %d. offset: %llu\n", size, offset);
		
		/* Closed File */
		set_fs(oldfs);
		filp_close(filp, 0);
		printk("Test: [INIT] Closed file\n");
	}
	
	/* Initialize spin_lock */
	spin_lock_init(&Device.lock);

	/* Initialize queue and call sbd_request when there are request */
	Queue = blk_init_queue(sbd_request, &Device.lock);
	if (Queue == NULL) {
		printk("Test: [INIT] Failed to initialize queue\n");
		unregister_blkdev(major_num, "sbd");
		vfree(Device.data);
		return -ENOMEM;
	}

	/* Set logical_block_size for the queue to use */
	blk_queue_logical_block_size(Queue, logical_block_size);

	/* Initialize the gendisk structure */
	Device.gd = alloc_disk(16);
	if (!Device.gd) {
		printk("Test: [INIT] Failed to allocate gendisk struct\n");
		unregister_blkdev(major_num, "sbd");
		vfree(Device.data);
		return -ENOMEM;
	}
	
	/* initialize cypto struct and set the key.
	 * ctrypto_alloc_cipher takes the name of a crypto driver, the type, 
	 * and a mask. */
	tfm = crypto_alloc_cipher("aes", 0, 0);

	if (IS_ERR(tfm))
		printk("Test: [INIT] Failed to allocate cipher\n");
	else
		printk("Test: [INIT] Allocated Cipher\n");

	/* debug info about crypto */
	printk("Test: [INIT] Block Cipher Size: %u\n", crypto_cipher_blocksize(tfm));
	printk("Test: [INIT] Crypto key: %s\n", key);
	printk("Test: [INIT] Key length: %d\n", keylen);
	
	/*
	 * And the gendisk structure.
	 */
	Device.gd->major = major_num;
	Device.gd->first_minor = 0;
	Device.gd->fops = &sbd_ops;
	Device.gd->private_data = &Device;
	strcpy(Device.gd->disk_name, "sbd0");
	set_capacity(Device.gd, nsectors);
	Device.gd->queue = Queue;
	
	/* Register the partition in Device.gd with the kernel */
	add_disk(Device.gd);

	printk("Test: [INIT] Successfully initialized\n");
	
	return 0;
}

static void __exit sbd_exit(void)
{
	struct file *filp = NULL;
	mm_segment_t oldfs;
	ssize_t size;
	unsigned long long offset = 0;

	printk("Test: [EXIT] Exiting\n");

	/* first write the data to the file */
	oldfs = get_fs();
	set_fs(get_ds());
	filp = filp_open("/Data", O_WRONLY | O_TRUNC | O_CREAT, S_IRWXUGO);

	if (IS_ERR(filp)) {
		printk("Test: [EXIT] Failed to open file\n");
		set_fs(oldfs);
	} else {
		printk("Test: [EXIT] Opened file\n");
		
		/* Write the bytes to the file */
		size = vfs_write(filp, Device.data, Device.size, &offset);
		printk("Test: [EXIT] Wrote to file: %d Offset: %llu.\n", size, offset);
		
		/* Closing File */
		set_fs(oldfs);
		filp_close(filp, 0);
		printk("Test: [EXIT] Closed file\n");
	}
	
	del_gendisk(Device.gd);
	put_disk(Device.gd);
	unregister_blkdev(major_num, "sbd");
	blk_cleanup_queue(Queue);
	vfree(Device.data);
	
	crypto_free_cipher(tfm);
	
	printk("Test: [EXIT] Module Exited\n");
}

module_init(sbd_init);
module_exit(sbd_exit);

MODULE_AUTHOR("Lance Umagat");
MODULE_DESCRIPTION("Testing Device Driver");
