#include "Decoder.hpp"

Decoder::Decoder(const GeneratorConfiguration *cfg):
  IN_FRAME_COUNT(cfg->fft_in_frame_count),
  DELTA_FRAME(cfg->delta_frame),
  OUT_SAMPLE_RATE(cfg->out_sample_rate)
{
  audio_stream_index_ = -1;
  av_format_ctx_ = nullptr;
  av_codec_ctx_ = nullptr;
  swr_ = nullptr;
  av_frame_ = nullptr;
  av_packet_ = nullptr;
}

Decoder::~Decoder()
{
  if (av_format_ctx_)
  {
    avformat_close_input(&av_format_ctx_);
    avformat_free_context(av_format_ctx_);
  }
  if (av_codec_ctx_)
    avcodec_free_context(&av_codec_ctx_);
  if (swr_)
    swr_free(&swr_);
  if (av_frame_)
    av_frame_free(&av_frame_);
  if (av_packet_)
    av_packet_free(&av_packet_);
}

// Open audio file and retrieve data from its header
void Decoder::openFile(const char *file_name) {
  av_format_ctx_ = avformat_alloc_context();
  if (!av_format_ctx_)
    throw std::runtime_error("Error: couldn't create AVFormatContext");
  if (avformat_open_input(&av_format_ctx_, file_name, nullptr, nullptr) != 0)
    throw std::runtime_error(std::string("Error: could not open file '%s'", file_name));
  // retrieve stream information from file header
  if (avformat_find_stream_info(av_format_ctx_, nullptr) < 0) {
    std::string error = std::string("Error: could not retrieve stream information from '%s'", file_name);
    throw std::runtime_error(error);
  }
}

// Initialize fields and structures necessary in decoding process
void Decoder::setup()
{
  initCodecContext();
  initSwrContext();
  initPacket();
  initFrame();
}

void Decoder::decode(sample_fmt **data, int *data_size) {
  allocateMemory(data);
  addOverlapPrefix(data, data_size);
  readFile(data, data_size);
  addOverlapSuffix(data, data_size);
}

// allocate memory prior to reading file contents
void Decoder::allocateMemory(sample_fmt **data) {
  *data = (sample_fmt *)malloc(FRAME_ALLOC_UNIT * sizeof(sample_fmt));
  if(!*data)
    throw std::runtime_error("Error: couldn't allocate memory");
}

void Decoder::readFile(sample_fmt **data, int *data_size)
{
  int frame_count = 0;
  long data_capacity = FRAME_ALLOC_UNIT;
  while (av_read_frame(av_format_ctx_, av_packet_) >= 0)
  {
    // if stream indexes do not match -> skip packet
    if (av_packet_->stream_index != audio_stream_index_)
    {
      av_packet_unref(av_packet_);
      continue;
    }
    // send packet to decoder
    int response = avcodec_send_packet(av_codec_ctx_, av_packet_);
    if (response < 0)
      throw std::runtime_error(std::string("Failed to decode packet: %s", avMakeError(response)));
    
    // receive decoded frame
    response = avcodec_receive_frame(av_codec_ctx_, av_frame_);
    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF)
    {
      av_packet_unref(av_packet_);
      continue;
    }
    else if (response < 0)
      throw std::runtime_error(std::string("Failed to decode packet: %s", avMakeError(response)));
    
    // resample frames
    uint8_t *buffer;
    av_samples_alloc(&buffer, nullptr, MONO, av_frame_->nb_samples, AV_SAMPLE_FMT_FLT, 0);
    frame_count = swr_convert(swr_, &buffer, av_frame_->nb_samples, (const uint8_t **)av_frame_->data, av_frame_->nb_samples);
    // reallocate memory when data array is too small to append a new buffer
    if(*data_size + frame_count > data_capacity) {
      data_capacity += FRAME_ALLOC_UNIT;
      reallocateMemory(data, data_capacity);
    }
    // append resampled frames to data
    memcpy(*data + *data_size, buffer, frame_count * sizeof(sample_fmt));
    *data_size += frame_count;
    
    // clean up
    av_freep(&buffer);
    av_packet_unref(av_packet_);
  }
}

// find and open required codec for file decoding
void Decoder::initCodecContext()
{
  AVCodecParameters *av_codec_params;
  AVCodec *av_codec;

  // find stream containing audio packets, save its index
  for (uint i = 0; i < av_format_ctx_->nb_streams; ++i)
  {
    av_codec_params = av_format_ctx_->streams[i]->codecpar;
    av_codec = avcodec_find_decoder(av_codec_params->codec_id);
    if (!av_codec)
      continue;
    if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO)
    {
      audio_stream_index_ = i;
      break;
    }
  }
  if (audio_stream_index_ == -1)
    throw std::runtime_error("Error: could not retrieve audio stream from file");

  // find & open codec
  av_codec_ctx_ = avcodec_alloc_context3(av_codec);
  if (!av_codec_ctx_) {
    throw std::runtime_error("Error: failed to create AVCodecContext");
  }
  if (avcodec_parameters_to_context(av_codec_ctx_, av_codec_params) < 0) {
    throw std::runtime_error("Error: couldn't initialize AVCodecContext");
  }
  if (avcodec_open2(av_codec_ctx_, av_codec, nullptr) < 0) {
    throw std::runtime_error("Error: couldn't open codec");
  }
}

// initialize and setup resampler
void Decoder::initSwrContext()
{
  swr_ = swr_alloc();
  
  av_opt_set_int(swr_, "in_channel_count", av_codec_ctx_->channels, 0);
  av_opt_set_int(swr_, "out_channel_count", 1, 0);
  av_opt_set_int(swr_, "in_channel_layout", av_codec_ctx_->channel_layout, 0);
  av_opt_set_int(swr_, "out_channel_layout", AV_CH_LAYOUT_MONO, 0);
  av_opt_set_int(swr_, "in_sample_rate", av_codec_ctx_->sample_rate, 0);
  av_opt_set_int(swr_, "out_sample_rate", OUT_SAMPLE_RATE, 0);
  av_opt_set_sample_fmt(swr_, "in_sample_fmt", av_codec_ctx_->sample_fmt, 0);
  av_opt_set_sample_fmt(swr_, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
  
  swr_init(swr_);
  if (!swr_is_initialized(swr_)) {
    throw std::runtime_error("Error: resampler has not been properly initialized");
  }
}

void Decoder::initPacket() {
  av_packet_ = av_packet_alloc();
  if (!av_packet_) {
    throw std::runtime_error("Error allocating AVPacket");
  }
}

void Decoder::initFrame() {
  av_frame_ = av_frame_alloc();
  if (!av_frame_) {
    throw std::runtime_error("Error allocating AVFrame");
  }
}

// append overlap 0's for first FFT input window
void Decoder::addOverlapPrefix(sample_fmt **data, int *data_size) {
  int frame_count = IN_FRAME_COUNT - DELTA_FRAME;
  memset(*data, 0, frame_count * sizeof(sample_fmt));
  *data_size += frame_count;
}

// append 0's to fill the last FFT input window
void Decoder::addOverlapSuffix(sample_fmt **data, int *data_size) {
  int data_capacity = *data_size / DELTA_FRAME; 
  data_capacity *= DELTA_FRAME;
  data_capacity += IN_FRAME_COUNT;
  reallocateMemory(data, data_capacity);
  memset(*data + *data_size, 0, (data_capacity - *data_size) * sizeof(sample_fmt));
  *data_size = data_capacity;
}

void Decoder::reallocateMemory(sample_fmt **data, int new_sample_capacity) {
  *data = (sample_fmt *)realloc(*data, new_sample_capacity * sizeof(sample_fmt));
  if(!*data) 
    throw std::runtime_error("Error: couldn't allocate memory");
}

const char *Decoder::avMakeError(int errnum)
{
  static char str[AV_ERROR_MAX_STRING_SIZE];
  memset(str, 0, sizeof(str));
  return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}